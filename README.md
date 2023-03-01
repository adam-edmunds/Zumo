# Zumo!!!


This is a Zumo search and rescue bot build for my Robotics module at Sheffield Hallam University.

# Tech Stack

## Backend

For this project I used Node.js which allowed me to use JavaScript for the backend. I used the `serialport` package to communicate with the Arduino. I also used the `socket.io` package to allow the frontend to communicate with the backend. I used the `express` package to create a server to host the frontend.

## Frontend

For this project I used React.js which allowed me to use JavaScript for the frontend. I used the `socket.io-client` package to allow the frontend to communicate with the backend. I also used `material-ui` to create the UI.

## Hardware

For this project we had to use a Zumo 32U4 robot with custom made board that had an XBee module on it. However, the XBee modules were very unreliable so we used Serial communication via usb cable.

# How to run

First you'll need to clone this repo. 

## Backend

To run the backend you'll need to install Node.js. Then you'll need to install the dependencies by running `npm install` in the `backend` directory. Then you can run the backend by running `node .` in the `backend` directory.

## Frontend

To run the frontend you'll need to install Node.js. Then you'll need to install the dependencies by running `npm install` in the `frontend` directory. Then you can run the frontend by running `npm start` in the `frontend` directory.

## Hardware

First install the Arduino IDE. Once done add an external board (https://www.pololu.com/docs/0J63/all#5.2). Once done install the `Zumo32U4` library via `Tools -> Manage Libraries`. Then upload the `Zumo32U4.ino` file to the Zumo.

<b>Note*:</b> Make sure to keep cable in while Zumo is running


# How to use

Once the backend and frontend are running you should see a message in the backend console saying 
```
Listening for requests on port 9001.
Serial Port /dev/tty.usbmodem1101 is opened.
React Client connected: uHwWIrqbQMNf0sM1AAAB
```

This means that the backend has successfully connected to the zumo via the serial port and the frontend via socket.io. You can then go to `localhost:3000` to see the frontend. 

By default the arduino doesn't do anything until it receives a command from the frontend. There are notifications for when the zumo sends back data however, if you wan't to see the list / live stream then make sure the checkbox `Show Log` is checked.

# Useful Links

- [Zumo 32U4](https://www.pololu.com/product/3126)
- [Node.js](https://nodejs.org/en/)
- [React.js](https://reactjs.org/)
- [Material-UI](https://material-ui.com/)
- [Socket.io](https://socket.io/)
- [Express](https://expressjs.com/)
- [Serialport](https://serialport.io/)
- [Arduino IDE](https://www.arduino.cc/en/software)