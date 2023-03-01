import express from 'express';
import { SerialPort } from 'serialport';
import { Server as SocketIO } from 'socket.io';

const app = express();

const SERVER_PORT = 9001;

// Two paths as I have to test on my windows pc and mac
// const path = 'COM5';
const path = '/dev/tty.usbmodem1101';

const arduinoPort = new SerialPort({
  path: path,
  baudRate: 9600,
});

const server = app.listen(SERVER_PORT, () =>
  console.log(`Listening for requests on port ${SERVER_PORT}.`)
);

const reactSocket = new SocketIO(server, { cors: { origin: '*' } });

// get from front-end send to arduino
reactSocket.on('connection', (clientSocket) => {
  console.log(`React Client connected: ${clientSocket.id}`);
  clientSocket.on('sentData', (message) => {
    arduinoPort.write(`${message}\n`);
  });
});

// getting data from arduino and send to front-end
arduinoPort.on('open', () => {
  console.log('Serial Port ' + arduinoPort.path + ' is opened.');
  arduinoPort.on('data', (data) => {
    console.log(`Zumo Data: ${data}`);
    reactSocket.emit('data', `${data}`);
  });
});
