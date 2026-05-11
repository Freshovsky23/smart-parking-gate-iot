import serial
import time
from datetime import datetime
from sqlalchemy import create_engine, Column, Integer, String, DateTime
from sqlalchemy.orm import declarative_base, sessionmaker

# ==========================================
# 1. DATABASE CONFIGURATION (PostgreSQL)
# ==========================================
# IMPORTANT: Replace these placeholders with your actual database credentials.
# Never hardcode real passwords in public repositories!
# Format: postgresql://USER:PASSWORD@HOST:PORT/DATABASE_NAME
DB_URL = "postgresql://YOUR_USER:YOUR_PASSWORD@127.0.0.1:5433/parking_db"

# Initialize SQLAlchemy engine and session
engine = create_engine(DB_URL)
Base = declarative_base()
Session = sessionmaker(bind=engine)
session = Session()

# Define the database table schema
class GateLog(Base):
    __tablename__ = 'parking_logs'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    event = Column(String(50))
    timestamp = Column(DateTime, default=datetime.now)

# Create the table in the database if it doesn't exist yet
Base.metadata.create_all(engine)

# ==========================================
# 2. ARDUINO CONNECTION SETUP
# ==========================================
# NOTE: Change 'COM9' to the actual port your Arduino is connected to.
# On Mac/Linux, this will look like '/dev/ttyUSB0' or '/dev/ttyACM0'.
PORT_ARDUINO = 'COM9' 
BAUD_RATE = 9600

try:
    # Attempt to establish a serial connection with the Arduino
    arduino = serial.Serial(PORT_ARDUINO, BAUD_RATE, timeout=1)
    print(f"Successfully connected to Arduino on port {PORT_ARDUINO}")
except Exception as e:
    print(f"Failed to connect to Arduino: {e}")
    print("Please check your USB connection and port name.")
    exit()

# ==========================================
# 3. MAIN LISTENING LOOP
# ==========================================
print("Listening for data from the parking gate...")

while True:
    try:
        # Check if there is any data waiting in the serial buffer
        if arduino.in_waiting > 0:
            # Read a line of text from USB, decode it, and remove whitespace/newlines
            usb_data = arduino.readline().decode('utf-8').strip()
            
            # Process the specific signals sent by the Arduino
            if usb_data == "GATE_OPENED":
                current_time = datetime.now().strftime('%H:%M:%S')
                print(f"[{current_time}] Vehicle detected! Saving entry to database...")
                
                # Create a new record and save it to the database
                new_entry = GateLog(event="ENTRY - GATE OPENED")
                session.add(new_entry)
                session.commit()
                
            elif usb_data == "GATE_CLOSED":
                current_time = datetime.now().strftime('%H:%M:%S')
                print(f"[{current_time}] Gate closed. Saving to database...")
                
                # Create a new record and save it to the database
                new_log = GateLog(event="GATE CLOSED")
                session.add(new_log)
                session.commit()
                
    except KeyboardInterrupt:
        # Gracefully handle the script termination (e.g., when pressing Ctrl+C)
        print("\nShutting down the system...")
        arduino.close()
        session.close()
        break