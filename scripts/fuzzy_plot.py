
import numpy as np
import skfuzzy as fuzz
import matplotlib.pyplot as plt
import pathlib as pl
import os
current_dir = pl.Path(__file__).parent.absolute()

def plot_BMP():
    range = np.arange(0, 100, .5)

    # Generate fuzzy membership functions
    neg = fuzz.trapmf(range, [0,0,0.5,10])
    low = fuzz.trimf(range, [0.5,10,50])
    medium = fuzz.trapmf(range, [10,50,250,500])
    high = fuzz.trapmf(range, [250,500,2000,2000])

    fake_neg = fuzz.trapmf(range, [0,0,0.5,10])
    fake_low = fuzz.trimf(range, [0.5,10,20])
    fake_medium = fuzz.trapmf(range, [10,20,30,50])
    fake_high = fuzz.trapmf(range, [30,50,100,100])

    # Visualize these universes and membership functions
    fig, ax = plt.subplots(figsize=(5, 3))
    ax.plot(range, fake_neg, 'c', linewidth=1.5, label='Neg.')
    ax.plot(range, fake_low, 'b', linewidth=1.5, label='Low')
    ax.plot(range, fake_medium, 'g', linewidth=1.5, label='Medium')
    ax.plot(range, fake_high, 'r', linewidth=1.5, label='High')
    #ax.set_title('BMP membership')
    ax.set_xticks([0.5,10, 20, 30,50,100]) 
    ax.set_xticklabels([0.5,10,50, 250,500,2000])
    ax.set_ylabel('Membership')
    ax.set_xlabel('Conc.($ng/ml$)')
    ax.legend()


    # Turn off top/right axes

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()

    plt.tight_layout()
    plt.savefig( os.path.join(current_dir,"BMP.png"))
plot_BMP()

def plot_TGF():
    range = np.arange(0, 15, .025)

    # Generate fuzzy membership functions
    neg = fuzz.trapmf(range, [0,0,0.015,0.025])
    low = fuzz.trimf(range, [0.015,0.025,15])
    high = fuzz.trimf(range, [0.025,15,15])
    fake_neg = fuzz.trapmf(range, [0,0,1,3])
    fake_low = fuzz.trimf(range, [1,3,15])
    fake_high = fuzz.trimf(range, [3,15,15])


    # Visualize these universes and membership functions
    fig, ax = plt.subplots(figsize=(5, 3))
    ax.plot(range, fake_neg, 'c', linewidth=1.5, label='Neg.')
    ax.plot(range, fake_low, 'b', linewidth=1.5, label='Low')
    ax.plot(range, fake_high, 'r', linewidth=1.5, label='High')
    #ax.set_title('BMP membership')
    ax.set_xticks([0,1,3,15]) 
    ax.set_xticklabels([0,0.015,0.025,15])
    ax.set_ylabel('Membership')
    ax.set_xlabel('Conc.($ng/ml$)')
    ax.legend()


    # Turn off top/right axes

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()

    plt.tight_layout()
    plt.savefig( os.path.join(current_dir,"TGF.png"))
plot_TGF()
def plot_MG():
    range = np.arange(0, 60, .5)

    # Generate fuzzy membership functions
    neg = fuzz.trapmf(range, [0,0,0.8,5])
    low = fuzz.trimf(range, [0.8,5,15])
    medium = fuzz.trimf(range, [5,15,40])
    high = fuzz.trapmf(range, [15,40,60,60])

    fake_neg = fuzz.trapmf(range, [0,0,3,8])
    fake_low = fuzz.trimf(range, [3,8,20])
    fake_medium = fuzz.trimf(range, [8,20,40])
    fake_high = fuzz.trapmf(range, [20,40,60,60])


    # Visualize these universes and membership functions
    fig, ax = plt.subplots(figsize=(5, 3))

    ax.plot(range, fake_neg,'c', linewidth=1.5, label='Neg.')
    ax.plot(range, fake_low, 'b', linewidth=1.5, label='Low')
    ax.plot(range, fake_medium, 'g', linewidth=1.5, label='Medium')
    ax.plot(range, fake_high, 'r', linewidth=1.5, label='High')
    #ax.set_title('BMP membership')
    ax.set_xticks([0,3,8,20,40,60]) 
    ax.set_xticklabels([0,0.8,r'$c_{Mg,l}$',r'$c_{Mg,m}$',r'$c_{Mg,h}$',60])
    ax.set_ylabel('Membership')
    ax.set_xlabel('Conc.(mM)')
    ax.legend()


    # Turn off top/right axes

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()

    plt.tight_layout()
    plt.savefig( os.path.join(current_dir,"Mg.png"))
plot_MG()

def plot_AE():
    range = np.arange(0, 1, .05)

    # Generate fuzzy membership functions
    low = fuzz.trimf(range, [0,0,1])
    high = fuzz.trimf(range, [0,1,1])

    # Visualize these universes and membership functions
    fig, ax = plt.subplots(figsize=(5, 3))

    ax.plot(range, low, 'b', linewidth=1.5, label='Low')
    ax.plot(range, high, 'r', linewidth=1.5, label='High')
    #ax.set_title('BMP membership')
    ax.set_xticks([0,1]) 
    ax.set_xticklabels([0,1])
    ax.set_ylabel('Membership')
    ax.set_xlabel('AE')
    ax.legend()


    # Turn off top/right axes

    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()

    plt.tight_layout()
    plt.savefig( os.path.join(current_dir,"AE.png"))
plot_AE()


def plot_CD():
    range = np.arange(0, 9, 1)

    # Generate fuzzy membership functions
    low = fuzz.trapmf(range, [0,0,1,2])
    medium = fuzz.trapmf(range, [1,2,5,7])
    high = fuzz.trapmf(range, [5,7,8,8])


    # Visualize these universes and membership functions
    fig, ax = plt.subplots(figsize=(5, 3))

    ax.plot(range, low, 'b', linewidth=1.5, label='Low')
    ax.plot(range, medium, 'g', linewidth=1.5, label='Medium')
    ax.plot(range, high, 'r', linewidth=1.5, label='High')
    #ax.set_title('BMP membership')
    ax.set_xticks([0, 1, 2,5,7,8]) 
    ax.set_xticklabels([0, 1, 2,r'$CD_{h}$-1',r'$CD_{h}$',8])
    ax.set_ylabel('Membership')
    ax.set_xlabel('Neighbor cells count')
    ax.legend()
    # Turn off top/right axes
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.get_xaxis().tick_bottom()
    ax.get_yaxis().tick_left()
    plt.tight_layout()
    plt.savefig( os.path.join(current_dir,"CD.png"))
plot_CD()