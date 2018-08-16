import os
import datetime

from sqlalchemy import Column, Integer, DateTime, Float, Boolean, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import create_engine

from sqlalchemy.orm import sessionmaker

Base = declarative_base()


class data(Base):
    __tablename__ = 'data'

    date = Column(DateTime, primary_key=True, default=datetime.datetime.utcnow)
    humidity = Column(Float, nullable=False)
    temperature = Column(Float, nullable=False)
    rain_hour = Column(Float, nullable=False)
    rain_day = Column(Float, nullable=False)
    pressure = Column(Float, nullable=False)
    wind_speed = Column(Float, nullable=False)
    wind_dir = Column(Integer, nullable=False)
    wind_gust = Column(Float, nullable=False)
    wind_gust_dir = Column(Integer, nullable=False)
    wind_avg = Column(Float, nullable=False)
    wind_avg_dir = Column(Integer, nullable=False)
    dewpoint = Column(Float, nullable=False)


class graphdata(Base):
    __tablename__ = 'graphdata'

    date = Column(DateTime, primary_key=True, default=datetime.datetime.utcnow)
    humidity = Column(Float, nullable=False)
    temperature = Column(Float, nullable=False)
    rain_hour = Column(Float, nullable=False)
    rain_day = Column(Float, nullable=False)
    pressure = Column(Float, nullable=False)
    wind_speed = Column(Float, nullable=False)
    wind_dir = Column(Integer, nullable=False)
    wind_gust = Column(Float, nullable=False)
    wind_gust_dir = Column(Integer, nullable=False)
    wind_avg = Column(Float, nullable=False)
    wind_avg_dir = Column(Integer, nullable=False)
    dewpoint = Column(Float, nullable=False)


class status(Base):
    __tablename__ = 'status'

    id = Column(Integer, primary_key=True)
    battery = Column(Float, nullable=False)
    is_charging = Column(Boolean, nullable=False)
    time_charging = Column(Integer, nullable=False)
    uptime = Column(Integer, nullable=False)
    resets = Column(Integer, nullable=False)
    lost_packets = Column(Integer, nullable=False)
    reporting = Column(Boolean, nullable=False)
    base_uptime = Column(Integer, nullable=False)
    base_resets = Column(Integer, nullable=False)


class commands(Base):
    __tablename__ = 'commands'

    id = Column(Integer, primary_key=True, autoincrement=True)
    command = Column(String(10), nullable=False)


class Database:

    def __init__(self, databse: str):
        self.engine = create_engine('sqlite:///{0}'.format(databse))

        Base.metadata.create_all(self.engine)
        Base.metadata.bind = self.engine

        self.session = sessionmaker(bind=self.engine)
