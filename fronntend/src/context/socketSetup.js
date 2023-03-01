import { createContext } from 'react';
import socketio from 'socket.io-client';

export const socket = socketio.connect('http://localhost:9001', {
  transport: ['websocket'],
});
export const SocketContext = createContext();
