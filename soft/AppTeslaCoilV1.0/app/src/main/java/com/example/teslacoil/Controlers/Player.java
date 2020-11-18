package com.example.teslacoil.Controlers;

import com.example.teslacoil.Controlers.MidiUtils.midi.MidiFile;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.ChannelEvent;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.Controller;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.MidiEvent;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.NoteOff;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.NoteOn;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.PitchBend;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.meta.EndOfTrack;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.meta.Tempo;
import com.example.teslacoil.View.MainActivity;

import java.util.ArrayList;
import java.util.List;

public class Player {
    public static final byte TX_MIDI_PLAYER_KEY = 0x01;
    public static final byte TX_MIDI_DATA_MSK = (byte) (0x80 | (TX_MIDI_PLAYER_KEY << 5));

    public static final byte TX_SET_START_PLAYER = TX_MIDI_DATA_MSK | 0x01;
    public static final byte TX_SET_STOP_PLAYER = TX_MIDI_DATA_MSK | 0x02;
    public static final byte TX_SET_RESET_PLAYER = TX_MIDI_DATA_MSK | 0x03;
    public static final byte TX_SET_CURRENT_TICK = TX_MIDI_DATA_MSK | 0x04;
    public static final byte TX_SET_TEMPO_PLAYER = TX_MIDI_DATA_MSK | 0x05;
    public static final byte TX_SET_RESOLUTION_PLAYER = TX_MIDI_DATA_MSK | 0x06;
    public static final byte TX_SET_MIDI_EVENT_PLAYER = TX_MIDI_DATA_MSK | 0x07;
    public static final byte TX_SET_MIDI_EVENT_TICK_PLAYER = TX_MIDI_DATA_MSK | 0x08;
    public static final byte TX_SET_MAX_TICKS_PLAYER = TX_MIDI_DATA_MSK | 0x09;
    public static final byte TX_SET_BEGIN_PLAYER = TX_MIDI_DATA_MSK | 0x0a;
    public static final byte TX_SET_VOLUME = TX_MIDI_DATA_MSK | 0x0b;
    public static final byte TX_SET_USEVELOCITY = TX_MIDI_DATA_MSK | 0x0c;
    public static final byte TX_SET_PORTAMENTO_TIME = TX_MIDI_DATA_MSK | 0x0d;
    public static final byte TX_SET_PORTAMENTO_TYPE = TX_MIDI_DATA_MSK | 0x0e;
    public static final byte TX_MAX_SIZE = TX_MIDI_DATA_MSK | 0x0f;
    public static final byte TX_SET_ATTACK = TX_MIDI_DATA_MSK | 0x10;
    public static final byte TX_SET_DECAY = TX_MIDI_DATA_MSK | 0x11;
    public static final byte TX_SET_SUSTAIN = TX_MIDI_DATA_MSK | 0x12;
    public static final byte TX_SET_REALEASE = TX_MIDI_DATA_MSK | 0x13;

    public static final byte RX_MIDI_PLAYER_KEY = 0x01;
    public static final byte RX_MIDI_DATA_MSK = (byte) (0x80 | (RX_MIDI_PLAYER_KEY << 5));

    public static final byte RX_SET_BEGIN_PLAYER = RX_MIDI_DATA_MSK | 0x01;
    public static final byte RX_SET_RESET_PLAYER = RX_MIDI_DATA_MSK | 0x02;
    public static final byte RX_LOAD_HALF_BUFFER_PLAYER = RX_MIDI_DATA_MSK | 0x03;
    public static final byte RX_START_PLAYER = RX_MIDI_DATA_MSK | 0x04;
    public static final byte RX_LOST_PACKAGE_PLAYER = RX_MIDI_DATA_MSK | 0x05;

    public final int METTA_EVENT_END_TRACK = 5;
    public final int METTA_EVENT_SET_TEMPO = 3;

    public final int SIZE_BUFFER = 146; // ~1kB

    int counterEvents = 0;
    private List<MidiEvent> midiEventList = new ArrayList<>();
    private long CurrentMaxTicks = 0;
    private int CurrentResolution = 0;

    public Player(List<MidiEvent> events, MidiFile file) {
        midiEventList = events;
        CurrentMaxTicks = file.getLengthInTicks();
        CurrentResolution = file.getResolution();
    }

    public Player(List<MidiEvent> events, long lengthTicks, int resolution) {
        midiEventList = events;
        CurrentMaxTicks = lengthTicks;
        CurrentResolution = resolution;
    }

    private void setTempo(float tempo) {
        int val = (int) (tempo * 10);
        byte data1 = (byte) (val >> 7);
        byte data2 = (byte) (val & 0x7f);

        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_TEMPO_PLAYER, data1, data2, 0});
    }

    private void setResolution(int resolution) {
        byte data1 = (byte) (resolution >> 7);
        byte data2 = (byte) (resolution & 0x7f);

        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_RESOLUTION_PLAYER, data1, data2, 0});
    }

    private void setMaxTicksPlayer(long ticks) {
        byte data1 = (byte) (ticks >> 14);
        byte data2 = (byte) ((ticks >> 7) & 0x7f);
        byte data3 = (byte) (ticks & 0x7f);

        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_MAX_TICKS_PLAYER, data1, data2, data3});
    }

    private void setMidiEvent(MidiEvent event) {
        byte data1 = 0, data2 = 0, data3 = 0, tickData1 = 0, tickData2 = 0, tickData3 = 0;

        if (event instanceof NoteOn ||
            event instanceof NoteOff ||
            event instanceof Controller ||
            event instanceof PitchBend) {

            data1 = (byte) (((((ChannelEvent)event).getType() - 8) << 4) | ((ChannelEvent)event).getChannel());
            data2 = (byte) ((ChannelEvent)event).getData1();
            data3 = (byte) ((ChannelEvent)event).getData2();

        } else if(event instanceof Tempo) {
            int val = (int) (((Tempo)event).getBpm() * 10);
            data1 = 0x70 | METTA_EVENT_SET_TEMPO;
            data2 = (byte) (val >> 7);
            data3 = (byte) (val & 0x7f);

        } else if(event instanceof EndOfTrack) {
            data1 = 0x70 | METTA_EVENT_END_TRACK;
        }

        tickData1 = (byte) (event.getTick() >> 14);
        tickData2 = (byte) ((event.getTick() >> 7) & 0x7f);
        tickData3 = (byte) (event.getTick() & 0x7f);

        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_MIDI_EVENT_PLAYER, data1, data2, data3});
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_MIDI_EVENT_TICK_PLAYER, tickData1, tickData2, tickData3});
    }

    public void writeEvents(int size) {
        for(int i = 0; (i < size) && (counterEvents < midiEventList.size()); i++) {
            setMidiEvent(midiEventList.get(counterEvents));
            counterEvents++;
        }
    }

    private void PlayerBegin() {
        counterEvents = 0;

        setResolution(CurrentResolution);
        setMaxTicksPlayer(CurrentMaxTicks);

        writeEvents(SIZE_BUFFER);
    }

    public void setStart() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_START_PLAYER, 0, 0, 0});
    }

    public void setStop() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_STOP_PLAYER, 0, 0, 0});
    }

    public void setReset() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_RESET_PLAYER, 0, 0, 0});
    }

    public void setCurrentTick(long tick) {
        if(MidiPlayerController.getInstance().isPlay()) {
            setStop();
            MidiPlayerController.getInstance().midiPlayerStatus.onPauseTrack();
        }

        setResolution(CurrentResolution);
        setMaxTicksPlayer(CurrentMaxTicks);

        if(tick > 0) {
            for(int i = midiEventList.size() - 1; i > -1; i--) {
                if(midiEventList.get(i).getTick() < tick && midiEventList instanceof Tempo) {
                    setTempo(((Tempo) midiEventList.get(i)).getBpm());
                    break;
                }
            }

            for(int i = midiEventList.size() - 1; i > -1; i--) {
                if(midiEventList.get(i).getTick() < tick) {
                    counterEvents = i;
                    break;
                }
            }

            byte data1 = (byte) (tick >> 14);
            byte data2 = (byte) ((tick >> 7) & 0x7f);
            byte data3 = (byte) (tick & 0x7f);

            Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_CURRENT_TICK, data1, data2, data3});

        } else {
            setTempo(Tempo.DEFAULT_BPM);
            counterEvents = 0;
            Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_SET_CURRENT_TICK, 0, 0, 0});
        }
    }

    public void readMidiData(byte[] data) {
        switch (data[0]) {
            default: break;

            case RX_SET_BEGIN_PLAYER: PlayerBegin(); break;
            case RX_LOAD_HALF_BUFFER_PLAYER:
                if(counterEvents < midiEventList.size()) writeEvents(SIZE_BUFFER / 2);
                else Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_MAX_SIZE, 0, 0, 0});
                break;

            case RX_SET_RESET_PLAYER:
                MainActivity.getInstance().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        MidiPlayerController.getInstance().setStop();
                    }
                }); break;

            case RX_LOST_PACKAGE_PLAYER:
                MainActivity.getInstance().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        MidiPlayerController.getInstance().PlayerLostPackage();
                    }
                }); break;

            case RX_START_PLAYER:
                writeEvents(SIZE_BUFFER);

                MainActivity.getInstance().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(MidiPlayerController.getInstance().isPlay()) {
                        setStart();
                        MidiPlayerController.getInstance().midiPlayerStatus.onStartTrack();
                    }
                }
            }); break;
        }
    }
}
