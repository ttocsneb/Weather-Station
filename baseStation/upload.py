import requests

#read the weather data
f = open(".weather", "r")

date = f.readline().rstrip()
humidity = f.readline().rstrip()
dewpoint = f.readline().rstrip()
temp = f.readline().rstrip()
rain = f.readline().rstrip()
rainday = f.readline().rstrip()
pressure = f.readline().rstrip()

wind_speed = f.readline().rstrip()
wind_dir = f.readline().rstrip()
wind_gust_speed = f.readline().rstrip()
wind_gust_dir = f.readline().rstrip()
wind_average_speed = f.readline().rstrip()
wind_average_dir = f.readline().rstrip()

f.close()


# create a string to hold the first part of the URL
WUurl = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?"
WU_station_id = "KUTPROVO29" # Replace XXXX with your PWS ID
WU_station_pwd = "5mfy6wkx" # Replace YYYY with your Password
WUcreds = "ID=" + WU_station_id + "&PASSWORD="+ WU_station_pwd
date_str = "&dateutc=" + date
action_str = "&action=updateraw"

request = (WUurl +
    WUcreds +
    date_str +
    "&humidity=" + humidity +
    "&dewptf=" + dewpoint +
    "&tempf=" + temp +
    "&rainin=" + rain + 
    "&dailyrainin=" + rainday +
    "&baromin=" + pressure +
    "&windspeedmph=" + wind_speed +
    "&winddir=" + wind_dir +
    "&windgustmph_10m=" + wind_gust_speed +
    "&windgustdir_10m=" + wind_gust_dir +
    "&windspdmph_avg2m=" + wind_average_speed +
    "&winddir_avg2m=" + wind_average_dir +
    action_str)

print("Test request: " + request)

#r = requests.get(request)

#print("Received " + str(r.status_code) + " " + str(r.text).rstrip())