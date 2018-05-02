import requests
import mysql.connector as sql
import urllib
import datetime

print("upload.py")

command = "SELECT * FROM data WHERE date = (SELECT MAX(date) FROM data)"

#connect to sql
connection = sql.connect(user='upload', database='weather')

# get the weather data
cursor = connection.cursor()
cursor.execute(command)
data = cursor.fetchone()
connection.close()

print(command)

#read the weather data

date = str(data[0])
humidity = "{:.1f}".format(data[1])
temp = "{:.2f}".format(data[2])
rain = "{:.2f}".format(data[3])
rainday = "{:.2f}".format(data[4])
pressure = "{:.4f}".format(data[5])

wind_speed = "{:.1f}".format(data[6])
wind_dir = str(data[7])
wind_gust_speed = "{:.1f}".format(data[8])
wind_gust_dir = str(data[9])
wind_average_speed = "{:.1f}".format(data[10])
wind_average_dir = str(data[11])

dewpoint = "{:.2f}".format(data[12])

print(date)
'''
print(humidity)
print(dewpoint)
print(temp)
print(rain)
print(rainday)
print(pressure)
print(wind_speed)
print(wind_dir)
print(wind_gust_speed)
print(wind_gust_dir)
print(wind_average_speed)
print(wind_average_dir)
'''

# create a string to hold the first part of the URL
WUurl = "https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?"
WU_station_id = "KUTPROVO29" # Replace XXXX with your PWS ID
WU_station_pwd = "5mfy6wkx" # Replace YYYY with your Password
WUcreds = "ID=" + WU_station_id + "&PASSWORD="+ WU_station_pwd
date_str = "&dateutc=" + urllib.parse.quote(date)
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