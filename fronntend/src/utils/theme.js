import { createTheme } from '@mui/material';

const darkTheme = createTheme({
  palette: {
    type: 'dark',
    primary: {
      main: '#91E5F6',
      title: '#fff',
      medium: '#43444D',
      darkMedium: '#33343B',
      salmon: '#FA8072',
      subtitle: '#D3D3D3',
      button: {
        border: '#0065e8',
        color: 'white',
        hover: {
          border: '#4C98FA',
          background: 'rgba(76, 152, 250, 0.4)',
        },
      },
      error: {
        background: '#160b0b',
        color: '#dcb3b3',
      },
    },
    secondary: {
      main: '#84D2F6',
    },
    inputColor: {
      main: '#fff',
    },
    error: {
      main: '#FF3B00',
    },
    background: {
      default: '#24252a',
    },
    paginationHighlight: {
      default: '#FF3B00',
    },
    text: {
      primary: '#fff',
    },
    notification: {
      main: 'red',
    },
  },
  typography: {
    allVariants: {
      fontFamily: "'Montserrat', sans-serif",
    },
  },
  components: {
    MuiDivider: {
      styleOverrides: {
        root: {
          '&::before': {
            borderTop: 'thin solid #CACACA',
          },
          '&::after': {
            borderTop: 'thin solid #CACACA',
          },
        },
      },
    },
    MuiOutlinedInput: {
      styleOverrides: {
        root: {
          color: '#fff',
        },
        notchedOutline: {
          borderColor: 'white !important',
        },
      },
    },
    MuiInput: {
      styleOverrides: {
        root: {
          '&.MuiInput-underline::before': {
            borderBottomColor: 'rgba(255,255,255,0.3)',
          },
          '&.MuiInput-underline:hover:not(.Mui-disabled):before': {
            borderBottomColor: 'rgba(255,255,255,0.5)',
          },
        },
      },
    },
    MuiInputBase: {
      styleOverrides: {
        input: {
          '&.MuiInput-input': {
            color: 'white',
          },
        },
      },
    },
    MuiInputLabel: {
      styleOverrides: {
        root: {
          color: 'white',
        },
      },
    },
    MuiCssBaseline: {
      styleOverrides: {
        body: {
          scrollbarColor: '#6b6b6b #2b2b2b',
          '&::-webkit-scrollbar, & *::-webkit-scrollbar': {
            backgroundColor: '#2b2b2b',
            width: 8,
          },
          '&::-webkit-scrollbar-thumb, & *::-webkit-scrollbar-thumb': {
            backgroundColor: '#6b6b6b',
            minHeight: 24,
          },
          '&::-webkit-scrollbar-thumb:focus, & *::-webkit-scrollbar-thumb:focus':
            {
              backgroundColor: '#959595',
            },
          '&::-webkit-scrollbar-thumb:active, & *::-webkit-scrollbar-thumb:active':
            {
              backgroundColor: '#959595',
            },
          '&::-webkit-scrollbar-thumb:hover, & *::-webkit-scrollbar-thumb:hover':
            {
              backgroundColor: '#959595',
            },
          '&::-webkit-scrollbar-corner, & *::-webkit-scrollbar-corner': {
            backgroundColor: '#2b2b2b',
          },
        },
      },
    },
    MuiListItem: {
      styleOverrides: {
        root: {
          transition: 'background-color 0.3s ease-in-out',
          '&:hover': {
            backgroundColor: '#75788A',
          },
        },
      },
    },
  },
  transitions: {
    easing: {
      // This is the most common easing curve.
      easeInOut: 'cubic-bezier(0.4, 0, 0.2, 1)',
      // Objects enter the screen at full velocity from off-screen and
      // slowly decelerate to a resting point.
      easeOut: 'cubic-bezier(0.0, 0, 0.2, 1)',
      // Objects leave the screen at full velocity. They do not decelerate when off-screen.
      easeIn: 'cubic-bezier(0.4, 0, 2, 1)',
      // The sharp curve is used by objects that may return to the screen at any time.
      sharp: 'cubic-bezier(0.4, 0, 0.6, 1)',
    },
    duration: {
      enteringScreen: 800,
    },
  },
});

export default darkTheme;
