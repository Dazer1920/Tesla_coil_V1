package com.example.teslacoil.Controlers;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Message;

import androidx.annotation.NonNull;

import com.example.teslacoil.Controlers.Utils.ProcessSystemData;

import static com.example.teslacoil.Utils.Pref.getData;

public class InterrupterController {
    private static InterrupterController interrupterController = new InterrupterController();

    public static InterrupterController getInstance() {
        return interrupterController;
    }

    public static final int PULSEWH_POSIT = 0;
    public static final int FREQ_POSIT = 1;
    public static final int DUTY_MOD_POSIT = 2;
    public static final int FREQ_MOD_POSIT = 3;

    public static final int ENABLE_INTER = 4;
    public static final int PULSE_INTER = 5;

    public static final int TX_INTERRUPTER_DATA_KEY = 0x02;
    private static final int TX_INTERRUPTER_MSK = 0x80 | (TX_INTERRUPTER_DATA_KEY << 5);

    public static final int TX_SET_PULSE_WHITH = TX_INTERRUPTER_MSK | 0x01;
    public static final int TX_SET_FREQUENCY = TX_INTERRUPTER_MSK | 0x02;
    public static final int TX_SET_MODULATER = TX_INTERRUPTER_MSK | 0x03;
    public static final int TX_SET_PULSE = TX_INTERRUPTER_MSK | 0x04;
    public static final int TX_SET_ENABLE = TX_INTERRUPTER_MSK | 0x05;

    public boolean isEnable;
    private int[] values = new int[4];

    public int[] max = new int[] {100, 400, 100, 10};
    public int[] min = new int[] {0, 20, 0, 1};
    public int[] fault = new int[] {50, 20, 100, 1};

    public static final int MAX_PULSE = 75;
    public static final int MIN_PULSE = 1;
    public static final float MAX_DUTY = 3.0f;
    public static final float MIN_DUTY = 0.01f;

    private Handler handler;

    private StateInterrupterController stateInterrupterController;

    public void setStateInterrupterController(StateInterrupterController state) {
        stateInterrupterController = state;
    }

    private void setMessage(int what) {
        Message msg = handler.obtainMessage(what);
        handler.sendMessage(msg);
    }

    public interface StateInterrupterController {
        public void onOffInterrupter();
        public void onOnInterrupter();
        public void setValue(int value, int code);
        public void onProtectOn();
        public void onProtectOff();
        public void endInit();
        public void initValues(int value, int code);
    }

    private int calculatePulse(int power) {
        int pw;
        if(power == 0) return 1;

        pw = (int) (MAX_PULSE * (((float) power) / ((float) max[PULSEWH_POSIT])));

        pw = (pw > MAX_PULSE)? MAX_PULSE : pw;
        pw = (pw < MIN_PULSE)? MIN_PULSE : pw;

        return pw;
    }

    public int getValue(int code) {
        return values[code];
    }

    @SuppressLint("HandlerLeak")
    public void startController() {
        isEnable = false;
        handler = new Handler() {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                Tesla.getInstance().restartScanValueDevice();

                switch (msg.what) {
                    default: break;

                    case PULSEWH_POSIT: TxSetPulseWhith(); break;
                    case FREQ_POSIT: TxSetFrequency(); break;

                    case DUTY_MOD_POSIT:
                    case FREQ_MOD_POSIT: TxSetModulater(); break;

                    case ENABLE_INTER:
                        if(isEnable) {
                            TxSetPulseWhith();
                            TxSetFrequency();
                            TxSetModulater();
                        }

                        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {(byte)
                                (TX_SET_ENABLE), (byte) ((isEnable)? 1 : 0), 0, 0});
                        break;

                    case PULSE_INTER:
                        int val = calculatePulse(values[PULSEWH_POSIT]);
                        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {(byte)
                                (TX_SET_PULSE), (byte) (val >> 7), (byte) (val & 0x7f), 0});
                        break;
                }
            }
        };

        setProtect();

        for(int i = 0; i < 4; i++) {
            int val = getData("KeyValSeecbarInrer" + i, fault[i]);
            values[i] = val;

            stateInterrupterController.setValue(val, i);
            stateInterrupterController.initValues(val - min[i], i);
        }

        stateInterrupterController.endInit();
    }

    public void setEnableInterrupter(Boolean enableInterrupter) {
        if(enableInterrupter) stateInterrupterController.onOnInterrupter();
        else stateInterrupterController.onOffInterrupter();

        isEnable = enableInterrupter;
        setMessage(ENABLE_INTER);
    }

    public void setPulse() {
        setMessage(PULSE_INTER);
    }

    public void setValueInterrupter(int value, int code) {
        values[code] = value + min[code];
        stateInterrupterController.setValue(values[code], code);
        setMessage(code);
    }

    public void setProtect() {
        if(ProcessSystemData.getInstance().isTermoProtect || ProcessSystemData.getInstance().isLowPower)
            stateInterrupterController.onProtectOn();
        else stateInterrupterController.onProtectOff();
    }

    private void TxSetPulseWhith() {
        int val = calculatePulse(values[PULSEWH_POSIT]);
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {(byte) TX_SET_PULSE_WHITH, (byte) (val >> 7), (byte) (val & 0x7f), 0});
    }

    private void TxSetFrequency() {
        int val = values[FREQ_POSIT];
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {(byte) TX_SET_FREQUENCY, (byte) (val >> 7), (byte) (val & 0x7f), 0});
    }

    private void TxSetModulater() {
        byte val1 = (byte) values[DUTY_MOD_POSIT];
        byte val2 = (byte) values[FREQ_MOD_POSIT];
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {(byte) TX_SET_MODULATER, val1, val2, 0});
    }
}
