import { CssBaseline, Slide } from '@mui/material';
import { ThemeProvider } from '@mui/system';
import React from 'react';
import ReactDOM from 'react-dom/client';
import { App } from './components';
import { socket, SocketContext } from './context/socketSetup';
import darkTheme from './utils/theme';
import { SnackbarProvider } from 'notistack';

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  //<React.StrictMode>
  <SocketContext.Provider value={socket}>
    <SnackbarProvider
      maxSnack={3}
      anchorOrigin={{
        vertical: 'top',
        horizontal: 'right',
      }}
      TransitionComponent={Slide}
      transitionDuration={{ exit: 200, enter: 200 }}
      autoHideDuration={1750}
      preventDuplicate
    >
      <ThemeProvider theme={darkTheme}>
        <CssBaseline />
        <App />
      </ThemeProvider>
    </SnackbarProvider>
  </SocketContext.Provider>
  // </React.StrictMode>
);
