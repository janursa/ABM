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
    static shared_ptr<base_model> & fuzzy_model() {
        static shared_ptr<base_model>  var{}; return var;
    };
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
        auto CHECK = [](vector<double> &data)->void{ //checks that the vector has ascending order
            
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
            std::vector<double> low{0, 0, params["CD_L_t"], params["CD_M_t1"]};
            std::vector<double> medium{params["CD_L_t"], params["CD_M_t1"],params["CD_M_t2"], params["CD_H_t"]};
            std::vector<double> high{params["CD_M_t2"], params["CD_H_t"], 1, 1};

            CHECK(low); CHECK(medium); CHECK(high);
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
            std::vector<double> low{0, 0,params["AE_L_t"], params["AE_H_t"]};
            std::vector<double> high{params["AE_L_t"],params["AE_H_t"], 1, 1};
            CHECK(low);CHECK(high);
            InputVariable *input2 = new InputVariable;
            input2->setName("AE");
            input2->setDescription("");
            input2->setEnabled(true);
            input2->setRange(0, 1);
            input2->setLockValueInRange(false);
            input2->addTerm(new Trapezoid("low", low[0], low[1], low[2], low[3]));
            input2->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input2);
        };
        // MG: 3 level
        auto INPUT_MG = [&]() {

            std::vector<double> negligible{0,0, params["MG_L_t1"]};
            std::vector<double> low{0, params["MG_L_t1"], params["MG_L_t2"], params["MG_H_t"]};
            // std::vector<double> medium{params["MG_L_t"], params["MG_M_t"], params["MG_H_t"]};
            std::vector<double> high{params["MG_L_t2"], params["MG_H_t"], params["Mg_max"], params["Mg_max"]};
            NORMALIZE(negligible,params["Mg_max"]); NORMALIZE(low,params["Mg_max"]); NORMALIZE(high,params["Mg_max"]);

            CHECK(negligible); CHECK(low);CHECK(high);// checks that the parameters have the right ascending order; this is useful to control the calibration params
            InputVariable *input2 = new InputVariable;
            input2->setName("Mg");
            input2->setDescription("");
            input2->setEnabled(true);
            input2->setRange(0, 1);
            input2->setLockValueInRange(false);
            input2->addTerm(new Triangle("negligible", negligible[0], negligible[1], negligible[2]));
            input2->addTerm(new Trapezoid("low", low[0], low[1], low[2], low[3]));
            // input2->addTerm(new Triangle("medium", medium[0],medium[1],medium[2]));
            input2->addTerm(new Trapezoid("high", high[0], high[1], high[2], high[3]));
            engine->addInputVariable(input2);
        };
        // Health: 2 level
        auto INNER_HEALTH = [&]() {
            OutputVariable *out1 = new OutputVariable;
            out1->setName("health");
            out1->setDescription("");
            out1->setEnabled(true);
            out1->setRange(0.000, 1.000);
            out1->setLockValueInRange(false);
            out1->setAggregation(fl::null);
            out1->setDefuzzifier(new WeightedAverage("Automatic"));
            out1->setDefaultValue(fl::nan);
            out1->setLockPreviousValue(false);
            out1->addTerm(new Constant("low", 0));
            out1->addTerm(new Constant("high", 1.000));
            engine->addOutputVariable(out1);
        };
        // ME: 3 level
        auto INNER_ME = [&]() {
            OutputVariable *out1 = new OutputVariable;
            out1->setName("Me");
            out1->setDescription("");
            out1->setEnabled(true);
            out1->setRange(0.000, 1.000);
            out1->setLockValueInRange(false);
            out1->setAggregation(fl::null);
            out1->setDefuzzifier(new WeightedAverage("Automatic"));
            out1->setDefaultValue(fl::nan);
            out1->setLockPreviousValue(false);
            out1->addTerm(new Constant("low", 0));
            out1->addTerm(new Constant("normal", params["Me_N_v"]));
            out1->addTerm(new Constant("high", 1));
            engine->addOutputVariable(out1);
        };
        // Pr: 3 levels
        auto OUTPUT_PROLIFERATION = [&]() {
            OutputVariable *out1 = new OutputVariable;
            out1->setName("Pr");
            out1->setDescription("");
            out1->setEnabled(true);
            out1->setRange(0.000, 1);
            out1->setLockValueInRange(false);
            out1->setAggregation(fl::null);
            out1->setDefuzzifier(new WeightedAverage("Automatic"));
            out1->setDefaultValue(fl::nan);
            out1->setLockPreviousValue(false);
            out1->addTerm(new Constant("low", 0.000));
            out1->addTerm(new Constant("normal", params["Pr_N_v"]));
            out1->addTerm(new Constant("high", 1));
            engine->addOutputVariable(out1);
        };
        // Mo: 2 levels
        auto OUTPUT_MORTALITY = [&]() {
            OutputVariable *out2 = new OutputVariable;
            out2->setName("Mo");
            out2->setDescription("");
            out2->setEnabled(true);
            out2->setRange(0, 1);
            out2->setLockValueInRange(false);
            out2->setAggregation(fl::null);
            out2->setDefuzzifier(new WeightedAverage("Automatic"));
            out2->setDefaultValue(fl::nan);
            out2->setLockPreviousValue(false);
            out2->addTerm(new Constant("low", 0));
            out2->addTerm(new Constant("high", 1));
            engine->addOutputVariable(out2);
        };
        // Mi: 2 levels
        auto OUTPUT_MIGRATION = [&]() {
            OutputVariable *out2 = new OutputVariable;
            out2->setName("Mi");
            out2->setDescription("");
            out2->setEnabled(true);
            out2->setRange(0, 1);
            out2->setLockValueInRange(false);
            out2->setAggregation(fl::null);
            out2->setDefuzzifier(new WeightedAverage("Automatic"));
            out2->setDefaultValue(fl::nan);
            out2->setLockPreviousValue(false);
            out2->addTerm(new Constant("low", 0));
            out2->addTerm(new Constant("high", 1));
            engine->addOutputVariable(out2);
        };
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

            /** inners **/
            INNER_HEALTH();
            INNER_ME();
            /** outputs **/
            OUTPUT_PROLIFERATION();
            this->output_tags.push_back("Pr");
            OUTPUT_MORTALITY();
            this->output_tags.push_back("Mo");
            OUTPUT_MIGRATION();
            this->output_tags.push_back("Mi");
            /** controller **/
            RuleBlock *mamdani = new RuleBlock;
            mamdani->setName("mamdani");
            mamdani->setDescription("");
            mamdani->setEnabled(true);
            mamdani->setConjunction(new AlgebraicProduct);
            mamdani->setDisjunction(new AlgebraicSum);
            mamdani->setImplication(new Minimum);
            mamdani->setActivation(new General);
            /***  health ***/
            mamdani->addRule(Rule::parse("if CD is low"
                                                 " or CD is high"
                                                 " or Mg is high"
                                                 " or AE is high"
                                                 " then health is low", engine));
            mamdani->addRule(Rule::parse("if CD is medium"
                                                 " and Mg is not high"
                                                 " and AE is not high"
                                                 " then health is high", engine));
            /***  metabolism ***/
            mamdani->addRule(Rule::parse("if health is high"
                                                 " and Mg is low"
                                                 " then Me is high", engine));
            mamdani->addRule(Rule::parse("if health is high"
                                                 " and Mg is not low"
                                                 " then Me is normal", engine));

            mamdani->addRule(Rule::parse("if health is low"
                                                 " then Me is low", engine));

            /***  Proliferation ***/
            mamdani->addRule(Rule::parse("if health is high"
                                                 " and Me is high"
                                                 " then Pr is high", engine));
            mamdani->addRule(Rule::parse("if health is high"
                                                 " and Me is normal"
                                                 " then Pr is normal", engine));
            mamdani->addRule(Rule::parse("if health is low"
                                                 " then Pr is low", engine));
            /***  mortality ***/
            mamdani->addRule(Rule::parse("if health is low"
                                                 " then Mo is high", engine));
            mamdani->addRule(Rule::parse("if health is high"
                                                 " then Mo is low", engine));
            /***  migration ***/
            mamdani->addRule(Rule::parse("if CD is high"
                                                 " then Mi is high", engine));
            mamdani->addRule(Rule::parse("if CD is not high"
                                                 " then Mi is low", engine));

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
    void tests();
};