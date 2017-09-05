module.exports = [
  {
    "type": "heading",
    "defaultValue": "Brigantine Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Mahalo."
  },
	
// STEPS SECTION	
	{
    "type": "section",
		"capabilities": ["HEALTH"],
    "items": [
			{
      	"type": "heading",
      	"defaultValue": "Step Tracking"
    	},
			{
  			"type": "toggle",
  			"messageKey": "ENABLE_STEPS",
  			"label": "Enable step tracking?",
  			"defaultValue": 1
			},
			{
  			"type": "radiogroup",
  			"messageKey": "STEPS_TYPE",
				"label": "I want my step count to be...",
				"defaultValue": 0,
  			"options": [
    			{ 
      			"label": "Better than yesterday",
      			"value": 0
    			},
    			{ 
      			"label": "Better than my average on this weekday",
						"value": 1
    			},
					{ 
      			"label": "Better than my daily average this week",
      			"value": 2
    			},
    			{ 
      			"label": "Better than my daily average this month",
						"value": 3
    			},
    			{ 
						"label": "Set manually",
      			"value": 4
    			}
  			]},
				{
				  "type": "input",
  				"messageKey": "STEPS_COUNT",
  				"defaultValue": "10000",
  				"label": "Manual Step Goal:",
  				"attributes": {
    				"placeholder": "10000",
    				"limit": 10,
    				"type": "steps"
  				}
				}
	]},
	
// SLEEP SECTION	
  {
    "type": "section",
		"capabilities": ["HEALTH"],
    "items": [
			{
      	"type": "heading",
      	"defaultValue": "Sleep Tracking",
    	},
			{
  			"type": "toggle",
  			"messageKey": "ENABLE_SLEEP",
  			"label": "Enable sleep tracking?",
  			"defaultValue": 1
			},
			{
  			"type": "radiogroup",
  			"messageKey": "SLEEP_TYPE",
				"label": "I want my sleep hours to be...",
				"defaultValue": 0,
  			"options": [
    			{ 
      			"label": "Better than yesterday", 
      			"value": 0
    			},
    			{ 
      			"label": "Better than my average on this weekday", 
      			"value": 1
    			},
    			{ 
      			"label": "Better than my daily average this week", 
      			"value": 2
    			},
    			{ 
      			"label": "Better than my daily average this month", 
      			"value": 3
    			},
    			{ 
						"label": "Set manually", 
      			"value": 4
    			}
				]},	
				{
			  	"type": "slider",
 			  	"messageKey": "SLEEP_COUNT",
					"label": "Manual Sleep Goal:",
  				"defaultValue": 8,
  				"min": 1,
  				"max": 24,
  				"step": 1
				}
	]},	
	
// SUBMIT	
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];