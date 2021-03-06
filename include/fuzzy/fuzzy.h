//
// Created by nourisaj on 1/6/20.
//

#pragma once
#include "fl/Headers.h"
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <cassert>
using namespace fl;
using namespace std;
vector<string> generate_rules(vector<vector<string>> factors, vector<string> levels, string target);
struct base_exception{
    base_exception(std::string msg):message(msg){

    }
    std::string message;
    const char *what() const throw() {
        return message.c_str();
    }
};
struct invalid_fuzzy_definition: public base_exception{
    invalid_fuzzy_definition(std::string msg):base_exception(msg){}
    
};
struct invalid_fuzzy_output: public base_exception{
    invalid_fuzzy_output(std::string msg):base_exception(msg){}
};
struct invalid_fuzzy_input: public base_exception{
    invalid_fuzzy_input(std::string msg):base_exception(msg){}
};
struct invalid_engine: public base_exception{
    invalid_engine(std::string msg):base_exception(msg){}
};
struct undefined_param_key: public base_exception{
    undefined_param_key(std::string msg):base_exception(msg){}
};
struct myMap{
    double operator [](std::string key){
        double value;
        try{value = m.at(key);}
        catch(std::out_of_range & e){
            std::cerr<<"Error: the key '"<<key<<"' is not defined in the params"<<endl;
            cout << e.what() << endl;
            std::terminate();
        }
        return value;
    }
    std::map<std::string,double> m;
    void insert(std::string key, double value){
        m[key] = value;
    }
};

struct base_model{
    base_model(){};
    
    base_model(myMap &params_):params(params_){
    };

    std::map<string,double> predict(std::map<std::string,double> & inputs_){
        std::string status;
        if (not engine->isReady(&status)) throw invalid_engine("[engine error] engine is not ready:n" + status);


        for (auto &input:inputs_){ // send in the inputs
            if (std::find(this->input_tags.begin(), this->input_tags.end(), input.first) == this->input_tags.end()){
                throw invalid_fuzzy_input("The input variable '"+ input.first+"' is not defined is the controller");
            }
            // cout<< input.first<<" "<<   input.second<<endl;
            engine->getInputVariable(input.first)->setValue(input.second);
            
        }
        engine->process();
        std::map<std::string,double> outputs;
        for (auto &tag:this->output_tags){
            double value = engine->getOutputVariable(tag)->getValue();
            if (isnan(value)) {
                string message =  "The value of fuzzy controller for " + tag + " is nun";
                throw invalid_fuzzy_output(message);
            }

            outputs.insert(std::pair<std::string,double>(tag,value));
        }
        return outputs;
    };

    virtual void initialize(){
        try {
            this->define();
        }catch(invalid_fuzzy_definition & ee){
            throw std::invalid_argument(ee.what());
        }
    }
    virtual void define()=0;
    Engine *engine;

    myMap params;
    vector<std::string> input_tags;
    vector<std::string> output_tags;
    
};
struct MSC_FUZZY:public base_model {
    MSC_FUZZY(myMap &params_):base_model(params_){
        this->initialize();
    };

    void restart() {
        engine->restart();
    }

    virtual void define() {
        auto ADJUST = [](vector<double> &data)->void{
            while (true){
                auto flag = false;
                for (unsigned i=0; i<data.size()-1;i++){
                    if (data[i]>data[i+1]){
                        data[i] = data[i+1];
                        flag = true;
                    }
                }
                if (flag == false) break;
            }
        };
        auto check_range = [](vector<double> &data)->void{ //checks that the vector has ascending order
            
            for (unsigned i=0; i<data.size()-1;i++){
                if (data[i]>data[i+1]){
                    // cout<<"data[i] "<<data[i]<<" data[i+1] "<<data[i+1]<<endl;
                    throw invalid_fuzzy_definition("Values of the fuzzy params doesnt match");
                }
            }

                
        };
        auto NORMALIZE = [](vector<double> &data, double max)->void{
                for (auto &item:data) item = item / max;
        };
        engine = new Engine;
        engine->setName("MSC");
        engine->setDescription("");
        // cell density: 3 levels
        auto INPUT_CELLDENSITY = [&]() {
            std::vector<double> low{0, 0, 0.111, 0.222};
            std::vector<double> medium{ 0.111, 0.222,params["CD_H_t"]-0.11, params["CD_H_t"]};
            std::vector<double> high{ params["CD_H_t"] - 0.11, params["CD_H_t"], 1, 1};

            check_range(low); check_range(medium); check_range(high);
            InputVariable *input1 = new InputVariable;
            input1->setName("CD");
            input1->setDescription("");
            input1->setEnabled(true);
            input1->setRange(0.000, 1.000);
            input1->setLockValueInRange(false);
            input1->addTerm(new Trapezoid("low", low[0], low[1], low[2], low[3]));
            input1->addTerm(new Trapezoid("medium", medium[0], medium[1], medium[2], medium[3]));
            input1->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input1);
        };
        // AE: 2 levels
        auto INPUT_AE = [&]() {
            // AE
            std::vector<double> low{0, 0,1};
            std::vector<double> high{0, 1, 1};
            //check_range(low);check_range(high);
            InputVariable *input = new InputVariable;
            input->setName("AE");
            input->setDescription("");
            input->setEnabled(true);
            input->setRange(0, 1);
            input->setLockValueInRange(false);
            input->addTerm(new Triangle("low", low[0], low[1], low[2]));
            input->addTerm(new Triangle("high", high[0], high[1], high[2]));
            engine->addInputVariable(input);
        };
        // Maturity: 2 levels
        auto INPUT_MATURITY = [&]() {
            std::vector<double> low{ 0, 0, 1 };
            std::vector<double> high{ 0, 1, 1 };
            InputVariable* input = new InputVariable;
            input->setName("maturity");
            input->setDescription("");
            input->setEnabled(true);
            input->setRange(0, 1);
            input->setLockValueInRange(false);
            input->addTerm(new Triangle("low", low[0], low[1], low[2]));
            input->addTerm(new Triangle("high", high[0], high[1], high[2]));
            engine->addInputVariable(input);
        };

        // Damage: 2 levels
        auto INPUT_DAMAGE = [&]() {
            std::vector<double> low{ 0, 0, 1 };
            std::vector<double> high{ 0, 1, 1 };
            InputVariable* input = new InputVariable;
            input->setName("damage");
            input->setDescription("");
            input->setEnabled(true);
            input->setRange(0, 1);
            input->setLockValueInRange(false);
            input->addTerm(new Triangle("low", low[0], low[1], low[2]));
            input->addTerm(new Triangle("high", high[0], high[1], high[2]));
            engine->addInputVariable(input);
        };
        // TGF: 3 level
        auto INPUT_TGF = [&]() {
            auto TGF_H_t = 43.3;
            std::vector<double> neg{ 0,0, 0.04,14 };
            std::vector<double> low{ 0.04,14, 36,TGF_H_t };
            std::vector<double> high{ 36,TGF_H_t, params["TGF_max"],params["TGF_max"] };
            NORMALIZE(neg, params["TGF_max"]); NORMALIZE(low, params["TGF_max"]); NORMALIZE(high, params["TGF_max"]);
            check_range(neg); check_range(low); check_range(high);// checks that the parameters have the right ascending order; this is useful to control the calibration params
            InputVariable* input2 = new InputVariable;
            input2->setName("TGF");
            input2->setDescription("");
            input2->setEnabled(true);
            input2->setRange(0, 1);
            input2->setLockValueInRange(false);
            input2->addTerm(new Trapezoid("neg", neg[0], neg[1], neg[2], neg[3]));
            input2->addTerm(new Trapezoid("low", low[0], low[1], low[2], low[3]));
            input2->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input2);
        };
        // BMP: 4 level
        auto INPUT_BMP = [&]() {
            std::vector<double> neg{ 0,0, 0.008,0.5 };
            std::vector<double> low{ 0.008,0.5, 10,50 };
            std::vector<double> medium{ 10, 50, 200,500 };
            std::vector<double> high{ 200,500, 2000, 2000 };
            NORMALIZE(neg, 2000); NORMALIZE(low, 2000); NORMALIZE(medium, 2000); NORMALIZE(high, 2000);
            check_range(neg); check_range(low); check_range(high);// checks that the parameters have the right ascending order; this is useful to control the calibration params
            InputVariable* input2 = new InputVariable;
            input2->setName("BMP");
            input2->setDescription("");
            input2->setEnabled(true);
            input2->setRange(0, 1);
            input2->setLockValueInRange(false);
            input2->addTerm(new Trapezoid("neg", neg[0], neg[1], neg[2], neg[3]));
            input2->addTerm(new Trapezoid("low", low[0], low[1], low[2], low[3]));
            input2->addTerm(new Trapezoid("medium", medium[0], medium[1], medium[2], medium[3]));
            input2->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input2);
        };
        // MG: 4 level
        auto INPUT_MG = [&]() {
            std::vector<double> neg{0,0, 0.8,params["MG_L_t"] };
            std::vector<double> low{ 0.8, params["MG_L_t"], params["MG_M_t"]};
             std::vector<double> medium{params["MG_L_t"], params["MG_M_t"], params["MG_H_t"]};
            std::vector<double> high{params["MG_M_t"], params["MG_H_t"], params["Mg_max"], params["Mg_max"]};
            NORMALIZE(neg,params["Mg_max"]); NORMALIZE(low,params["Mg_max"]); NORMALIZE(medium, params["Mg_max"]); NORMALIZE(high,params["Mg_max"]);
            check_range(neg); check_range(low); check_range(medium); check_range(high);// checks that the parameters have the right ascending order; this is useful to control the calibration params
            InputVariable *input2 = new InputVariable;
            input2->setName("Mg");
            input2->setDescription("");
            input2->setEnabled(true);
            input2->setRange(0, 1);
            input2->setLockValueInRange(false);
            input2->addTerm(new Trapezoid("neg", neg[0], neg[1], neg[2], neg[3]));
            input2->addTerm(new Triangle("low", low[0], low[1], low[2]));
             input2->addTerm(new Triangle("medium", medium[0],medium[1],medium[2]));
            input2->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input2);
        };
        
        // Pr: 5 levels
        auto OUTPUT_PROLIFERATION = [&]() {
            OutputVariable *out = new OutputVariable;
            out->setName("Pr");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0.000, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        // early Diff: 5 levels
        auto OUTPUT_EARLYDIFF = [&]() {
            OutputVariable* out = new OutputVariable;
            out->setName("earlyDiff");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0.000, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        // late Diff: 5 levels
        auto OUTPUT_LATEDIFF = [&]() {
            OutputVariable* out = new OutputVariable;
            out->setName("lateDiff");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0.000, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        // Mo: 5 levels
        auto OUTPUT_MORTALITY = [&]() {
            OutputVariable *out = new OutputVariable;
            out->setName("Mo");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        // Mi: 2 levels
        auto OUTPUT_MIGRATION = [&]() {
            OutputVariable *out = new OutputVariable;
            out->setName("Mi");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("low", 0));
            out->addTerm(new Constant("high", 1));
            engine->addOutputVariable(out);
        };
        /*
        // ECM prod: 5 levels
        auto OUTPUT_ECMPROD = [&]() {
            OutputVariable* out = new OutputVariable;
            out->setName("ECMprod");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        // HA prod: 5 levels
        
        auto OUTPUT_HAPROD = [&]() {
            OutputVariable* out = new OutputVariable;
            out->setName("HAprod");
            out->setDescription("");
            out->setEnabled(true);
            out->setRange(0, 1);
            out->setLockValueInRange(false);
            out->setAggregation(fl::null);
            out->setDefuzzifier(new WeightedAverage("Automatic"));
            out->setDefaultValue(fl::nan);
            out->setLockPreviousValue(false);
            out->addTerm(new Constant("verylow", 0.000));
            out->addTerm(new Constant("low", 0.25));
            out->addTerm(new Constant("medium", 0.5));
            out->addTerm(new Constant("high", 0.75));
            out->addTerm(new Constant("veryhigh", 1));
            engine->addOutputVariable(out);
        };
        */
        auto FORMULATION = [&]() {
            /*
             * input_tagsgs: cell density & Mg & AE
             * Output: Pr & Mo & Mi & adaptation
             * */
            /** input_tags **/
            INPUT_CELLDENSITY();
            this->input_tags.push_back("CD");
            INPUT_MG();
            this->input_tags.push_back("Mg");
            INPUT_AE();
            this->input_tags.push_back("AE");
            INPUT_DAMAGE();
            this->input_tags.push_back("damage");
            INPUT_BMP();
            this->input_tags.push_back("BMP");
            INPUT_TGF();
            this->input_tags.push_back("TGF");
            INPUT_MATURITY();
            this->input_tags.push_back("maturity");
            /** outputs **/
            OUTPUT_PROLIFERATION();
            this->output_tags.push_back("Pr");
            OUTPUT_MORTALITY();
            this->output_tags.push_back("Mo");
            OUTPUT_MIGRATION();
            this->output_tags.push_back("Mi");
            OUTPUT_EARLYDIFF();
            this->output_tags.push_back("earlyDiff");
            OUTPUT_LATEDIFF();
            this->output_tags.push_back("lateDiff");
            /*
            OUTPUT_ECMPROD();
            this->output_tags.push_back("ECMprod");
            OUTPUT_HAPROD();
            this->output_tags.push_back("HAprod");
            */
            /** controller **/
            RuleBlock *mamdani = new RuleBlock;
            mamdani->setName("mamdani");
            mamdani->setDescription("");
            mamdani->setEnabled(true);
            mamdani->setConjunction(new AlgebraicProduct);
            mamdani->setDisjunction(new AlgebraicSum);
            mamdani->setImplication(new Minimum);
            mamdani->setActivation(new General);
            /***  Proliferation ***/
            
            auto FORMULATE_PR = [&]() {
                // factors are organized in (favourable , non-favourable) formats
                vector<vector<string>> factors = {
                {"Mg is low", "Mg is not low"},
                {"BMP is low", "BMP is neg","BMP is high"},
                {"TGF is low", "TGF is not low"} };

                mamdani->addRule(Rule::parse("if (damage is not high and CD is not high and AE is not high and maturity is low) "
                    " and Mg is low and BMP is low and TGF is low "
                    " then Pr is veryhigh", engine));
                mamdani->addRule(Rule::parse("if (damage is not high and CD is not high and AE is not high and maturity is low) "
                    " and (Mg is low and BMP is low and TGF is not low) or (Mg is low and BMP is not low and TGF is low) or (Mg is not low and BMP is low and TGF is low) "
                    " then Pr is high", engine));
                mamdani->addRule(Rule::parse("if (damage is not high and CD is not high and AE is not high and maturity is low) "
                    " and (Mg is low and BMP is not low and TGF is not low) or (Mg is not low and BMP is not low and TGF is low) or (Mg is not low and BMP is low and TGF is not low) "
                    " then Pr is medium", engine));
                mamdani->addRule(Rule::parse("if (damage is not high and CD is not high and AE is not high) "
                    " and (maturity is high or (Mg is not low and BMP is not low and TGF is not low) )"
                    " then Pr is low", engine));
                mamdani->addRule(Rule::parse("if damage is high or CD is high or AE is high then Pr is verylow", engine));
            };
            FORMULATE_PR();
            
            /*auto FORMULATE_EARLYDIFF = [&]() {
                vector<vector<string>> factors = {
                {" CD is high", " CD is not high "},
                 {" Mg is low "," Mg is not low "},
                {" BMP is high", " BMP is not high "},
                {" TGF is high "," TGF is not high "} 
                };
                vector<string> levels = { "veryhigh", "high", "medium", "low", "verylow" };
                auto rules = generate_rules(factors, levels, "earlyDiff");
                for (auto& rule : rules) {
                    mamdani->addRule(Rule::parse(rule.c_str(), engine));
                }
                mamdani->addRule(Rule::parse("if damage is high then earlyDiff is verylow", engine));
            };*/
            auto FORMULATE_DIFF = [&](vector<vector<string>> factors,string key) {
                
                // very high : all 4
                string set_VH = factors[0][0] + " and " + factors[1][0] + " and " + factors[2][0] + " and " + factors[3][0];
                string rule = " if damage is not high and " + set_VH +
                    " then " + key + " is veryhigh";
                mamdani->addRule(Rule::parse(rule.c_str(), engine));
                // high: any 3
                string set_H1 = " ( "+factors[0][0] + " and " + factors[1][0] + " and " + factors[2][0] + " and " + factors[3][1]+" ) ";
                string set_H2 = " ( " + factors[0][0] + " and " + factors[1][0] + " and " + factors[2][1] + " and " + factors[3][0] + " ) ";
                string set_H3 = " ( " + factors[0][0] + " and " + factors[1][1] + " and " + factors[2][0] + " and " + factors[3][0] + " ) ";
                string set_H4 = " ( " + factors[0][1] + " and " + factors[1][0] + " and " + factors[2][0] + " and " + factors[3][0] + " ) ";
                rule = " if damage is not high and ( " + set_H1 + " or " + set_H2 + " or " + set_H3 + " or " + set_H4 +
                    " ) then  " + key + " is high";
                mamdani->addRule(Rule::parse(rule.c_str(), engine));
                // medium: any 2
                string set_M1 = " ( " + factors[0][0] + " and " + factors[1][0] + " and " + factors[2][1] + " and " + factors[3][1] + " ) ";
                string set_M2 = " ( " + factors[0][0] + " and " + factors[1][1] + " and " + factors[2][1] + " and " + factors[3][0] + " ) ";
                string set_M3 = " ( " + factors[0][0] + " and " + factors[1][1] + " and " + factors[2][0] + " and " + factors[3][1] + " ) ";
                string set_M4 = " ( " + factors[0][1] + " and " + factors[1][1] + " and " + factors[2][0] + " and " + factors[3][0] + " ) ";
                string set_M5 = " ( " + factors[0][1] + " and " + factors[1][0] + " and " + factors[2][1] + " and " + factors[3][0] + " ) ";
                string set_M6 = " ( " + factors[0][1] + " and " + factors[1][0] + " and " + factors[2][0] + " and " + factors[3][1] + " ) ";
                rule = " if damage is not high and ( " + set_M1 + " or " + set_M2 + " or " + set_M3 + " or " + set_M4 + " or " + set_M5 + " or " + set_M6 +
                    " ) then " + key + " is medium";
                mamdani->addRule(Rule::parse(rule.c_str(), engine));
                // low: any 1
                string set_L1 = " ( " + factors[0][0] + " and " + factors[1][1] + " and " + factors[2][1] + " and " + factors[3][1] + " ) ";
                string set_L2 = " ( " + factors[0][1] + " and " + factors[1][0] + " and " + factors[2][1] + " and " + factors[3][1] + " ) ";
                string set_L3 = " ( " + factors[0][1] + " and " + factors[1][1] + " and " + factors[2][0] + " and " + factors[3][1] + " ) ";
                string set_L4 = " ( " + factors[0][1] + " and " + factors[1][1] + " and " + factors[2][1] + " and " + factors[3][0] + " ) ";
                rule = " if damage is not high and ( " + set_L1 + " or " + set_L2 + " or " + set_L3 + " or " + set_L4 +
                    " ) then " + key + " is low";
                mamdani->addRule(Rule::parse(rule.c_str(), engine));
                // verylow: none or damage
                rule = " if damage is not high and ( " + factors[0][1] + " and " + factors[1][1] + " and " + factors[2][1] + " and " + factors[3][1] +
                    " ) then " + key + " is verylow";
                mamdani->addRule(Rule::parse(rule.c_str(), engine));

                mamdani->addRule(Rule::parse("if damage is high then " + key + " is verylow", engine));
            };
            vector<vector<string>> earlyDiff_factors = {
                {" CD is high", " CD is not high "},
                 {" Mg is neg "," Mg is not neg " },
                {" BMP is not neg", " BMP is neg "},
                {" TGF is not neg "," TGF is neg "}
            };
            FORMULATE_DIFF(earlyDiff_factors,"earlyDiff");
            vector<vector<string>> lateDiff_factors = {
                {" CD is high", " CD is not high "},
                 {" Mg is neg "," Mg is not neg " },
                {" BMP is not neg", " BMP is neg "},
                {"  TGF is neg "," TGF is not neg  "}
            };
            FORMULATE_DIFF(lateDiff_factors, "lateDiff");
            
            /***  mortality ***/
            auto FORMULATE_MO = [&]() {
                vector<vector<string>> factors = {
                    {"Mg is high","Mg is not high"},
                    {"CD is high or CD is low","CD is medium"},
                    {"TGF is neg","TGF is not neg"},
                    {"BMP is high","BMP is not high"}
                };
                // either damage or AE or all others
                mamdani->addRule(Rule::parse("if (damage is high or AE is high) "
                    " or (Mg is high and CD is high and TGF is neg and BMP is high)"
                    " then Mo is veryhigh", engine));
                // any combination of 3
                mamdani->addRule(Rule::parse("if (damage is low and AE is low) and ( "
                    " (Mg is high and CD is high and TGF is neg and BMP is not high)"
                    " or (Mg is high and CD is high and TGF is not neg and BMP is high)"
                    " or (Mg is high and CD is not high and TGF is neg and BMP is high)"
                    " or (Mg is not high and CD is high and TGF is neg and BMP is high)"
                    " ) then Mo is high", engine));
                // any combination of 2
                mamdani->addRule(Rule::parse("if (damage is low and AE is low) and ( "
                    " (Mg is high and CD is high and TGF is not neg and BMP is not high)"
                    " or (Mg is high and CD is not high and TGF is neg and BMP is not high)"
                    " or (Mg is high and CD is not high and TGF is not neg and BMP is high)"
                    " or (Mg is not high and CD is high and TGF is neg and BMP is not high)"
                    " or (Mg is not high and CD is high and TGF is not neg and BMP is high)"
                    " or (Mg is not high and CD is not high and TGF is neg and BMP is high)"
                    " ) then Mo is medium", engine));
                // any combination of 1
                mamdani->addRule(Rule::parse("if (damage is low and AE is low) and ( "
                    " (Mg is high and CD is not high and TGF is not neg and BMP is not high)"
                    " or (Mg is not high and CD is high and TGF is not neg and BMP is not high)"
                    " or (Mg is not high and CD is not high and TGF is neg and BMP is not high)"
                    " or (Mg is not high and CD is not high and TGF is not neg and BMP is high)"
                    " ) then Mo is low", engine));
                // none
                mamdani->addRule(Rule::parse("if (damage is low and AE is low) and "
                    " (Mg is not high and CD is not high and TGF is not neg and BMP is not high)"
                    " then Mo is verylow", engine));
            };
            FORMULATE_MO();
            
            ///***  HA prod ***/
            //auto FORMULATE_HAPROD = [&]() {
            //    vector<vector<string>> factors = {
            //        {"Mg is neg","Mg is not neg"},
            //        {"AE is not high","AE is high"},
            //        {"TGF is not high","TGF is high"},
            //        {"BMP is not neg","BMP is neg"}
            //    };
            //    // all 4
            //    mamdani->addRule(Rule::parse("if (damage is low and maturity is high) and "
            //        " (Mg is neg and AE is not high and TGF is not high and BMP is not neg) "
            //        " then HAprod is veryhigh", engine));
            //    // any 3
            //    mamdani->addRule(Rule::parse("if (damage is low and maturity is high) and ("
            //        " (Mg is neg and AE is not high and TGF is not high and BMP is neg) "
            //        " or (Mg is neg and AE is not high and TGF is high and BMP is not neg) "
            //        " or (Mg is neg and AE is high and TGF is not high and BMP is not neg) "
            //        " or (Mg is not neg and AE is not high and TGF is not high and BMP is not neg) "
            //        " ) then HAprod is high", engine));
            //    // any 2
            //    mamdani->addRule(Rule::parse("if (damage is low and maturity is high) and ( "
            //        " (Mg is neg and AE is not high and TGF is high and BMP is neg) "
            //        " or (Mg is neg and AE is high and TGF is not high and BMP is neg) "
            //        " or (Mg is neg and AE is high and TGF is high and BMP is not neg) "
            //        " or (Mg is not neg and AE is not high and TGF is not high and BMP is neg) "
            //        " or (Mg is not neg and AE is not high and TGF is high and BMP is not neg) "
            //        " or (Mg is not neg and AE is high and TGF is not high and BMP is not neg) "
            //        " ) then HAprod is medium", engine));
            //    // any 1
            //    mamdani->addRule(Rule::parse("if (damage is low and maturity is high) and ( "
            //        " (Mg is neg and AE is high and TGF is high and BMP is neg) "
            //        " or (Mg is not neg and AE is not high and TGF is high and BMP is neg) "
            //        " or (Mg is not neg and AE is high and TGF is not high and BMP is neg) "
            //        " or (Mg is not neg and AE is high and TGF is high and BMP is not neg) "
            //        " ) then HAprod is low", engine));
            //    // none
            //    mamdani->addRule(Rule::parse("if damage is high or maturity is low "
            //        " or (Mg is not neg and AE is high and TGF is high and BMP is neg)"
            //        " then HAprod is verylow", engine));
            //};
            //FORMULATE_HAPROD();
            ///***  ECM prod ***/
            //mamdani->addRule(Rule::parse(
            //    " if maturity is high and TGF is high"
            //    " then ECMprod is veryhigh"
            //    , engine));
            //mamdani->addRule(Rule::parse(
            //    " if maturity is high and TGF is not high"
            //    " then ECMprod is high"
            //    , engine));
            //mamdani->addRule(Rule::parse(
            //    " if maturity is not high and TGF is high"
            //    " then ECMprod is medium"
            //    , engine));
            //mamdani->addRule(Rule::parse(
            //    " if maturity is not high and TGF is not high"
            //    " then ECMprod is low"
            //    , engine));
            //mamdani->addRule(Rule::parse(
            //    " if damage is high"
            //    " then ECMprod is verylow"
            //    , engine));
            /***  migration ***/
            mamdani->addRule(Rule::parse(
                "if CD is high"
                " then Mi is high", engine));
            mamdani->addRule(Rule::parse(
                "if CD is not high"
                " then Mi is low", engine));
           
            //

            
            
            engine->addRuleBlock(mamdani);

            auto SELECTIVE_CHECK = [&](){
                vector<string> target_input = {"CD"};
                vector<string> target_output = {"Mo"};
                // map<string,double> non_target_inputs = { {"CD",0.5},{"AE",0}};
                map<string,double> non_target_inputs = { {"Mg",0.5},{"AE",0}};
                unsigned steps = 10;
                std::function<void(unsigned)> RECURSIVE = [&](unsigned int j){
                    
                    for (unsigned i = 0; i < steps; i++) {
                        auto input_tag  = target_input[target_input.size()-j];
                        engine->getInputVariable(input_tag)->setValue(engine->getInputVariable(input_tag)->getMinimum() +
                                i * (engine->getInputVariable(input_tag)->range() / steps));
                        if (j > 1){        
                            RECURSIVE(j -1); 
                        }
                        else{
                            for (auto &non_target:non_target_inputs){
                                auto tag = non_target.first;
                                auto value = non_target.second;
                                // cout<<tag<<" "<<value<<endl;
                                engine->getInputVariable(tag)->setValue(value);
                            }
                            engine->process();
                            for (auto &input_tag:target_input){
                                cout<<setw(4)<< input_tag << ": "<<setw(4)<<engine->getInputVariable(input_tag)->getValue() <<" ";
                            }
                            
                            for (auto &output_tag:target_output){
                                cout<<setw(4)<<output_tag <<": "<<setw(4)<<Op::str(engine->getOutputVariable(output_tag)->getValue())<<" ";
                            }
             
                            cout<<endl;
                            
                        }
                    
                    };
                };
                RECURSIVE(target_input.size());


            };
            
            // SELECTIVE_CHECK();
                

        };
        FORMULATION();
    }

};

struct fuzzy{
    fuzzy() {};
    fuzzy(std::string controller_name, std::map<std::string,double> params) ;
    std::map<std::string,double> predict(std::map<std::string,double> inputs) ;
    shared_ptr<base_model> fuzzy_model;
};
