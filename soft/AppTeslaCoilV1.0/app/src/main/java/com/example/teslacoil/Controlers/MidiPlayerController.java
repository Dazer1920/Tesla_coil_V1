package com.example.teslacoil.Controlers;

import com.example.teslacoil.Controlers.MidiUtils.midi.MidiReader;
import com.example.teslacoil.Controlers.Utils.ProcessSystemData;

import java.io.File;

import static com.example.teslacoil.Controlers.Player.TX_SET_ATTACK;
import static com.example.teslacoil.Controlers.Player.TX_SET_DECAY;
import static com.example.teslacoil.Controlers.Player.TX_SET_PORTAMENTO_TIME;
import static com.example.teslacoil.Controlers.Player.TX_SET_PORTAMENTO_TYPE;
import static com.example.teslacoil.Controlers.Player.TX_SET_REALEASE;
import static com.example.teslacoil.Controlers.Player.TX_SET_SUSTAIN;
import static com.example.teslacoil.Controlers.Player.TX_SET_USEVELOCITY;
import static com.example.teslacoil.Controlers.Player.TX_SET_VOLUME;
import static com.example.teslacoil.Utils.Pref.getData;
import static com.example.teslacoil.Utils.Pref.saveData;

public class MidiPlayerController {
    private static MidiPlayerController midiPlayerController = new MidiPlayerController();

    public static MidiPlayerController getInstance() {
        return midiPlayerController;
    }

    private MidiPlayerController() {}

    private static final String DIRECTORY_FILE_KEY = "dirFile";
    public File currentFile;
    public MidiPlayerStatus midiPlayerStatus;
    public MidiReader midiReader;
    public Player player;
    private boolean isPlay = false;
    public int currentTimeTrack = 0;
    public boolean isHaveFileDir = false;

    public interface MidiPlayerStatus {
        public void onStart();
        public void onPause();
        public void onStop();
        public void onReadFile();
        public void onReadedFile(boolean fail);
        public void onStartTrack();
        public void onPauseTrack();
        public void lostPackage();
        public void onProtectOn();
        public void onProtectOff();
    }

    public void setMidiPlayerStatus(MidiPlayerStatus midiPlayerStatus) {
        this.midiPlayerStatus = midiPlayerStatus;
    }

    public void startPlayer() {
        if(currentFile != null) {
            midiPlayerStatus.onReadedFile(false);
            isHaveFileDir = true;
            return;
        }

        String d = getData(DIRECTORY_FILE_KEY, "");
        if(d != null && !d.equals("")) {
            openFile(d);
            isHaveFileDir = true;
        } else isHaveFileDir = false;

        setProtect();
    }

    public void openFile(String dirrectory) {
        currentFile = new File(dirrectory);
        midiPlayerStatus.onReadFile();

        currentTimeTrack = 0;
        midiReader = new MidiReader(currentFile);
        midiReader.setResultReadFile(new MidiReader.ResultReadFile() {
            @Override
            public void fault() {
                midiPlayerStatus.onReadedFile(true);

                String d = getData(DIRECTORY_FILE_KEY, "");
                if(d != null && !d.equals("")) {
                    openFile(d);
                }
            }

            @Override
            public void readed() {
                player = new Player(midiReader.getMidiEventList(), midiReader.getCurrentFile());
                saveData(DIRECTORY_FILE_KEY, currentFile.getAbsolutePath());
                midiPlayerStatus.onReadedFile(false);
            }
        });
        midiReader.startReadFile();
    }

    public boolean isPlay() {
        return isPlay;
    }

    public void setStop() {
        isPlay = false;
        midiPlayerStatus.onStop();
        if(player != null) player.setReset();
    }

    public void setPlay() {
        isPlay = true;
        midiPlayerStatus.onStart();
        if(player != null) player.setStart();
    }

    public void setPause() {
        isPlay = false;
        midiPlayerStatus.onPause();
        if(player != null) player.setStop();
    }

    public void setTickMidiControllerMs(long Ms) {
        if(player != null && midiReader != null) {
            currentTimeTrack = (int) Ms;
            long tick = (Ms * midiReader.getCurrentFile().getLengthInTicks()) / midiReader.getTimeMs();
            player.setCurrentTick(tick);
        }
    }

    public void setVolume(byte vol) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_VOLUME, vol, 0, 0});
    }

    public void setUseVelocity(boolean use) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_USEVELOCITY, (byte) ((use)? 1 : 0), 0, 0});
    }

    public void setPortamentoTime(byte time) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_PORTAMENTO_TIME, time, 0, 0});
    }

    public void setPortamentoType(boolean type) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_PORTAMENTO_TYPE, (byte) ((type)? 1 : 0), 0, 0});
    }

    public void setAttack(byte val) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_ATTACK, val, 0, 0});
    }

    public void setDecay(byte val) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_DECAY, val, 0, 0});
    }

    public void setSustain(byte val) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_SUSTAIN, val, 0, 0});
    }

    public void setRealese(byte val) {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_REALEASE, val, 0, 0});
    }

    public void PlayerLostPackage() {
        if(midiPlayerStatus != null) midiPlayerStatus.lostPackage();
    }

    public void setProtect() {
        if(ProcessSystemData.getInstance().isTermoProtect || ProcessSystemData.getInstance().isLowPower)
            midiPlayerStatus.onProtectOn();
        else midiPlayerStatus.onProtectOff();
    }

    public void clearePlay() {
        isPlay = false;
    }
}
