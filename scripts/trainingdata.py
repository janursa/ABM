trainingData = {
	"IDs": [ "H2017_Mg0","H2017_Mg3","H2017_Mg6","H2017_Mg12","H2017_Mg60"],
	#"IDs": ["H2017_Mg0"],
	#"IDs": [ "B2016_C","B2016_M"],
	"scale": 0.025,
	"H2017_Mg0": {
		"setup": {
			"exp_duration": 72,
			"grid": {
				"area": 1,
				"volume": 0.274,
				"patch_size": 0.0074
			},
			"patch": {
				"attrs": {
					"Mg": 0,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 2739,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},

		"expectations": {
			"timepoints": [ "24", "48", "72" ],
			"24": {
				"liveCellCount": 3287,
				"viability": "0.95  1"
			},
			"48": {
				"liveCellCount": 7123,
				"viability": "0.95  1"
			},
			"72": {
				"liveCellCount": 10136,
				"viability": "0.95  1"
			}
		}
	},
	"H2017_Mg3": {
		"setup": {
			"exp_duration": 72,
			"grid": {
				"area": 1,
				"volume": 0.274,
				"patch_size": 0.0074
			},
			"patch": {
				"attrs": {
					"Mg": 3.6,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 2739,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "24", "48", "72" ],
			"24": {
				"liveCellCount": 3424,
				"viability": "0.9  1"
			},
			"48": {
				"liveCellCount": 6575,
				"viability": "0.9  1"
			},
			"72": {
				"liveCellCount": 13150,
				"viability": "0.9  1"
			}
		}
	},
	"H2017_Mg6": {
		"setup": {
			"exp_duration": 72,
			"grid": {
				"area": 1,
				"volume": 0.274,
				"patch_size": 0.0074
			},
			"patch": {
				"attrs": {
					"Mg": 6,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 2739,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "24", "48", "72" ],
			"24": {
				"liveCellCount": 3013,
				"viability": "0.85  .95"
			},
			"48": {
				"liveCellCount": 6301,
				"viability": "0.85  .95"
			},
			"72": {
				"liveCellCount": 9863,
				"viability": "0.85  .95"
			}
		}
	},
	"H2017_Mg12": {
		"setup": {
			"exp_duration": 72,
			"grid": {
				"area": 1,
				"volume": 0.274,
				"patch_size": 0.0074
			},
			"patch": {
				"attrs": {
					"Mg": 12,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 2739,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "24", "48", "72" ],
			"24": {
				"liveCellCount": 2191,
				"viability": "0.8  .9"
			},
			"48": {
				"liveCellCount": 4931,
				"viability": "0.8  .9"
			},
			"72": {
				"liveCellCount": 7123,
				"viability": "0.8  .9"
			}
		}
	},
	"H2017_Mg60": {
		"setup": {
			"exp_duration": 72,
			"grid": {
				"area": 1,
				"volume": 0.274,
				"patch_size": 0.0074
			},
			"patch": {
				"attrs": {
					"Mg": 60,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 2739,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "24", "48", "72" ],
			"24": {
				"liveCellCount": 1917,
				"viability": "0.6  .75"
			},
			"48": {
				"liveCellCount": 2739,
				"viability": "0.6  .75"
			},
			"72": {
				"liveCellCount": 2739,
				"viability": "0.6  .75"
			}
		}
	},
	"B2016_C": {
		"setup": {
			"exp_duration": 504,
			"grid": {
				"area": 1,
				"volume": 0.313,
				"patch_size": 0.013
			},
			"patch": {
				"attrs": {
					"Mg": 0,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 5208,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "168", "336", "504" ],
			"168": {
				"DNA":0.0842,
				"viability": "0.90  1",
				"OC": 0.53,
				"ALP": 0.30
			},
			"336": {
				"DNA":0.0579,
				"viability": "0.90  1",
				"OC": 0.71,
				"ALP": 0.53
			},
			"504": {
				"DNA":0.0648,
				"viability": "0.90  1",
				"OC": 0.8,
				"ALP": 0.57
			}
		}
	},
	"B2016_M": {
		"setup": {
			"exp_duration": 504,
			"grid": {
				"area": 1,
				"volume": 0.313,
				"patch_size": 0.013
			},
			"patch": {
				"attrs": {
					"Mg": 5,
					"pH": 7.8
				}
			},
			"agents": {
				"n": {
					"MSC": 5208,
					"Dead": 0
				},
				"MSC": {
					"attrs": {
						"pH": 7.8,
						"MI": 0
					}
				},
				"Dead": {
					"attrs": {}
				}
			}
		},
		"expectations": {
			"timepoints": [ "168", "336", "504" ],
			"168": {
				"DNA":0.0864,
				"viability": "0.90  1",
				"OC": 0.28,
				"ALP": 0.34
			},
			"336": {
				"DNA":0.0782,
				"viability": "0.90  1",
				"OC": 0.22,
				"ALP": 0.39
			},
			"504": {
				"DNA":0.0616,
				"viability": "0.90  1",
				"OC": 0.27,
				"ALP": 0.61
			}
		}
	}





}
