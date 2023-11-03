# Hash-Thermostat
Braiins OS Hash Thermostat to regulate indoor temperature by turning miner between pause and reusme


Hash Thermostat Guide
Step 1:
	Connect to the WIFI access point “AP_Thermostat” the password is “StackSats!”
	Note: WIFI access point deactivates when rebooted and device connects to network

Step 2:
	Open your web browser and navigate to http://10.0.0.10/  	
	
	-Login prompt the username and password are “admin”
	click “Sign In”



Step 3:
	Enter the desired thermostat behavior using the 	sliders and click “Apply Settings”



Step 4:
	Navigate to “Network Parameters”
	
	-Enter Wifi SSID and Password and click “submit” 
	 	If network has connected you should see the Status change to “Connected” otherwise 			double check the credentials

	-Enter the Miner Static IP and click “submit” 
		If the miner is connected the Status changes to “Hashing” or “Off” depending on the 			state
	
	-Click the Time Zone dropdown to select your Time Zone and click “submit” 

	When all looks good, click “Main Page”

Step 5: 
	Change the Login Password

Step 6: 
	If you have an MQTT broker you want to send data to click “MQTT Parameters”

Step 7:
	Click the “Reboot” Button
	You should be able to find the device by typing “http://thermostat.local/” in the browser
