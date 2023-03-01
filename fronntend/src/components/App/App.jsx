import {
  Button,
  Checkbox,
  Container,
  Grid,
  Stack,
  Typography,
} from '@mui/material';
import { useContext, useEffect, useState } from 'react';
import { SocketContext } from '../../context/socketSetup';
import { useSnackbar } from 'notistack';

import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import ArrowUpwardIcon from '@mui/icons-material/ArrowUpward';
import ArrowDownwardIcon from '@mui/icons-material/ArrowDownward';
import ArrowForwardIcon from '@mui/icons-material/ArrowForward';
import PersonIcon from '@mui/icons-material/Person';
import DirectionsCarIcon from '@mui/icons-material/DirectionsCar';
import SmartToyIcon from '@mui/icons-material/SmartToy';
import StopIcon from '@mui/icons-material/Stop';
import PanToolIcon from '@mui/icons-material/PanTool';
import RotateLeftIcon from '@mui/icons-material/RotateLeft';
import RotateRightIcon from '@mui/icons-material/RotateRight';

const CustomButton = ({
  children,
  onClick,
  size = 4,
  inLine = false,
  color = '#C9C9C9',
  hoverColor = '#626262',
}) => {
  return (
    <Grid
      item
      xs={size}
      minHeight='10vh'
      onClick={onClick}
      component={Button}
      sx={{
        backgroundColor: color,
        width: '50px',
        color: 'black',
        fontWeight: 'bold',
        marginRight: inLine ? 4 : 0,
        '&:hover': {
          backgroundColor: hoverColor,
        },
      }}
    >
      {children}
    </Grid>
  );
};

export const App = () => {
  const [toggleObjectCollision, setToggleObjectCollision] = useState(false);
  const [zumoLog, setZumoLog] = useState([]);
  const [showLog, setShowLog] = useState(false);

  const socket = useContext(SocketContext);

  const { enqueueSnackbar } = useSnackbar();

  const sendDataToArduino = (data) => {
    console.log('Sent data to arduino: ' + data);
    socket.emit('sentData', data);
  };

  useEffect(() => {
    socket.on('data', (data) => {
      console.log(data);
      setZumoLog((prev) => [...prev, data]);
      if (data === '') return;
      if (
        data.startsWith('ERROR') ||
        data.startsWith('Object') ||
        data.startsWith('Corner') ||
        data.startsWith('Left and Right')
      ) {
        enqueueSnackbar(data, { variant: 'error' });
      }
    });
  }, []);

  const handleLogClick = () => {
    setShowLog(!showLog);
  };

  return (
    <Container maxWidth={false}>
      <Typography variant='h3' textAlign='center' mt={6}>
        Zumo Bot Controls
      </Typography>
      <Stack alignItems='center' direction='row' justifyContent='center'>
        <Typography textAlign='center'>
          <Checkbox type='checkbox' onClick={handleLogClick} />
          Show Log
        </Typography>
      </Stack>
      <Grid
        container
        alignItems='center'
        justifyContent='center'
        textAlign='center'
        mt={4}
      >
        <Grid item xs={4} />
        <CustomButton size={3} onClick={() => sendDataToArduino('w')}>
          <Stack alignItems='center'>
            <ArrowUpwardIcon />
            <Typography>Forward</Typography>
          </Stack>
        </CustomButton>

        <Grid item xs={4} />
        <CustomButton size={3} onClick={() => sendDataToArduino('a')}>
          <Stack alignItems='center'>
            <ArrowBackIcon />
            <Typography>Left</Typography>
          </Stack>
        </CustomButton>

        <Grid item xs={4} />

        <CustomButton size={3} onClick={() => sendDataToArduino('d')}>
          <Stack alignItems='center'>
            <ArrowForwardIcon />
            <Typography>Right</Typography>
          </Stack>
        </CustomButton>

        <Grid item xs={4} />

        <CustomButton size={3} onClick={() => sendDataToArduino('s')}>
          <Stack alignItems='center'>
            <ArrowDownwardIcon />
            <Typography>Back</Typography>
          </Stack>
        </CustomButton>
        <Grid item xs={4} />
      </Grid>
      <Grid container alignItems='center' justifyContent='center' mt={6}>
        <CustomButton size={3} onClick={() => sendDataToArduino('l')} inLine>
          <Stack alignItems='center'>
            <RotateLeftIcon />
            <Typography>90° Left</Typography>
          </Stack>
        </CustomButton>
        <CustomButton size={3} onClick={() => sendDataToArduino('r')}>
          <Stack alignItems='center'>
            <RotateRightIcon />
            <Typography>90° Right</Typography>
          </Stack>
        </CustomButton>
      </Grid>
      <Grid container alignItems='center' justifyContent='center' mt={6}>
        <CustomButton
          size={2}
          onClick={() => sendDataToArduino('manual')}
          inLine
        >
          <Stack alignItems='center'>
            <PersonIcon style={{ fontSize: '2.25rem' }} />
            <Typography>Manual Mode</Typography>
          </Stack>
        </CustomButton>

        <CustomButton size={2} onClick={() => sendDataToArduino('semi')} inLine>
          <Stack alignItems='center'>
            <SmartToyIcon style={{ fontSize: '2.25rem' }} />
            <Typography>Semi Auto Mode</Typography>
          </Stack>
        </CustomButton>

        <CustomButton size={2} onClick={() => sendDataToArduino('auto')}>
          <Stack alignItems='center'>
            <DirectionsCarIcon style={{ fontSize: '2.25rem' }} />
            <Typography>Auto Mode</Typography>
          </Stack>
        </CustomButton>
      </Grid>
      <Grid container alignItems='center' justifyContent='center' mt={6} mb={6}>
        <CustomButton size={2} onClick={() => sendDataToArduino('x')} inLine>
          <Stack alignItems='center'>
            <StopIcon />
            <Typography>STOP!</Typography>
          </Stack>
        </CustomButton>

        <CustomButton
          size={2}
          onClick={() => {
            sendDataToArduino('o');
            setToggleObjectCollision(!toggleObjectCollision);
          }}
          color={toggleObjectCollision ? '#BAFD9B' : '#FD9B9B'}
          hoverColor={toggleObjectCollision ? '#60C133' : '#BB2B2B'}
        >
          <Stack alignItems='center'>
            <PanToolIcon />
            <Typography>Toggle Object Collision</Typography>
          </Stack>
        </CustomButton>
      </Grid>
      {showLog && (
        <Stack alignItems='center' justifyContent='center'>
          <Typography textAlign='center' variant='h4'>
            Zumo Log
            {zumoLog.map((log, index) => (
              <Typography key={index} textAlign='center'>
                {log}
              </Typography>
            ))}
          </Typography>
        </Stack>
      )}
    </Container>
  );
};
