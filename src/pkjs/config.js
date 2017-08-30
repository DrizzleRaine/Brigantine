module.exports = [
  {
    "type": "heading",
    "defaultValue": "Pip-Boy 300 Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Remember - your Pip-Boy is not a toy. Proper cleaning and maintenance will ensure its operation for this and future generations."
  },

	{
    "type": "section",
    "items": [
			{
      	"type": "heading",
      	"defaultValue": "Anxiety Mode"
    	},
			{
				"type": "text",
				"defaultValue": "If you feel up for it, choose the point where your Vault Boy's limbs are crippled. When a stat drops below the percentage indicated, you'd better hope you've got some stimpaks nearby."
			},
			{
  			"type": "toggle",
  			"messageKey": "CRIPPLED_STATUS",
  			"label": "Show crippled status?",
  			"defaultValue": 0
			},
			{
		  	"type": "slider",
 		  	"messageKey": "BATTERY_BREAKPOINT",
				"label": "Battery Low:",
  			"defaultValue": 30,
  			"min": 10,
  			"max": 90,
  			"step": 10
			},
			{
		  	"type": "slider",
 		  	"messageKey": "DEAD_BATTERY_BREAKPOINT",
				"label": "Battery Dead:",
  			"defaultValue": 10,
  			"min": 10,
  			"max": 90,
 				"step": 10
			},
			{
		  	"type": "slider",
 		  	"messageKey": "STEPS_BREAKPOINT",
				"label": "Steps Percentage:",
  			"defaultValue": 50,
  			"min": 10,
  			"max": 90,
  			"step": 10
			},
			{
		  	"type": "slider",
 		  	"messageKey": "SLEEP_BREAKPOINT",
				"label": "Sleep Percentage:",
  			"defaultValue": 80,
  			"min": 10,
  			"max": 90,
  			"step": 10
			}
	]},
	
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
		
  {
    "type": "section",
		"capabilities": ["HEALTH"],
    "items": [
			{
      	"type": "heading",
      	"defaultValue": "Heart Rate Tracking"
    	},
			{
  			"type": "toggle",
  			"messageKey": "ENABLE_HR",
  			"label": "Enable heart rate tracking?",
  			"defaultValue": 1
			}
	]},
	
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];