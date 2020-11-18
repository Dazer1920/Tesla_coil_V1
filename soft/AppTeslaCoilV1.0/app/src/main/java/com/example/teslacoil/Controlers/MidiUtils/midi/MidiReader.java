package com.example.teslacoil.Controlers.MidiUtils.midi;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Message;

import androidx.annotation.NonNull;

import com.example.teslacoil.Controlers.MidiUtils.midi.event.Controller;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.MidiEvent;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.NoteOff;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.NoteOn;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.PitchBend;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.meta.EndOfTrack;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.meta.Tempo;
import com.example.teslacoil.Controlers.MidiUtils.midi.util.MidiUtil;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import static com.example.teslacoil.Controlers.MidiUtils.midi.MidiFile.READ_FILE_FAULT;
import static com.example.teslacoil.Controlers.MidiUtils.midi.MidiFile.READ_FILE_OK;

public class MidiReader {
    private MidiFile currentFile;
    private File file;
    private boolean[] enableTracks;
    private Handler handler;
    private ResultReadFile resultReadFile;
    private List<MidiEvent> midiEventList = new ArrayList<>();

    private MidiTrackEventQueue[] midiEventQueues;

    private int mMpqn;
    private long mTick;
    private long oldTime = 0, oldTick = 0;
    private int trackCount = 0;
    private long timeMs;

    public long getTimeMs() {
        return timeMs;
    }

    public List<MidiEvent> getMidiEventList() {
        return midiEventList;
    }

    public interface ResultReadFile {
        public void fault();
        public void readed();
    }

    public MidiFile getCurrentFile() {
        return currentFile;
    }

    public boolean[] getEnableTracks() {
        return enableTracks;
    }

    public void setResultReadFile(ResultReadFile resurt) {
        resultReadFile = resurt;
    }

    @SuppressLint("HandlerLeak")
    public MidiReader(File file) {
        this.file = file;
        midiEventList.clear();

        handler = new Handler() {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);

                switch (msg.what) {
                    default: break;

                    case READ_FILE_OK: resultReadFile.readed(); break;
                    case READ_FILE_FAULT: resultReadFile.fault(); break;
                }
            }
        };
    }

    public void setEnableTracks(boolean[] e) {
        if(e.length != enableTracks.length || currentFile == null || currentFile.getResultCode() == READ_FILE_FAULT) return;

        enableTracks = new boolean[currentFile.getTrackCount()];
        for(int i = 0; i < enableTracks.length; i++) enableTracks[i] = e[i];

        midiEventList.clear();
        new Thread(new Runnable() {
            @Override
            public void run() {
                readMidiFile();
            }
        }).start();
    }

    public void startReadFile() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    currentFile = new MidiFile(file);
                    if(currentFile.getResultCode() == READ_FILE_OK) {
                        enableTracks = new boolean[currentFile.getTrackCount()];
                        for(int i = 0; i < enableTracks.length; i++) enableTracks[i] = true;
                        readMidiFile();
                    } else handler.sendEmptyMessage(READ_FILE_FAULT);
                } catch (IOException e) {
                    e.printStackTrace();
                    handler.sendEmptyMessage(READ_FILE_FAULT);
                }
            }
        }).start();
    }

    private void setMidiEvent(MidiEvent event, boolean isEnable) {
        if(isEnable) {
            if (event instanceof NoteOn ||
                    event instanceof NoteOff ||
                    event instanceof Controller ||
                    event instanceof PitchBend ||
                    event instanceof EndOfTrack ||
                    event instanceof Tempo) {
                midiEventList.add(event);
            }
        } else {
            if (event instanceof EndOfTrack || event instanceof Tempo) {
                midiEventList.add(event);
            }
        }
    }

    protected void dispatch(MidiEvent event, boolean isEnable) {
        if(event.getClass().equals(Tempo.class)) {
            Tempo t = (Tempo) event;

            if(((Tempo) event).getMpqn() != mMpqn) {
                timeMs -= oldTime;
                timeMs += MidiUtil.ticksToMs(t.getTick() - oldTick, mMpqn,
                        currentFile.getResolution());

                oldTick = t.getTick();
                mMpqn = t.getMpqn();

                oldTime = MidiUtil.ticksToMs(mTick - oldTick, mMpqn,
                        currentFile.getResolution());
                timeMs += oldTime;
            }
        }

        setMidiEvent(event, isEnable);
    }

    private void readMidiFile() {
        if(currentFile == null) {
            handler.sendEmptyMessage(READ_FILE_FAULT);
            return;
        }

        List<MidiTrack> tracks = currentFile.getTracks();
        midiEventQueues = new MidiTrackEventQueue[tracks.size()];
        for(int i = 0; i < tracks.size(); i++) {
            midiEventQueues[i] = new MidiTrackEventQueue(tracks.get(i));
        }

        trackCount = 0;

        oldTick = 0;
        mMpqn = Tempo.DEFAULT_MPQN;
        mTick = currentFile.getLengthInTicks();
        timeMs = MidiUtil.ticksToMs(mTick,
                Tempo.DEFAULT_MPQN, currentFile.getResolution());
        oldTime = timeMs;

        for(long tick = 0; tick < currentFile.getLengthInTicks(); tick++) {
            for(int t = 0; t < currentFile.getTrackCount(); t++) {
                MidiTrackEventQueue queue = midiEventQueues[t];
                if(!queue.hasMoreEvents()) {
                    continue;
                }

                ArrayList<MidiEvent> events = queue.getNextEventsUpToTick(tick);
                for(MidiEvent event : events) {
                    dispatch(event, enableTracks[t]);
                }
            }
        }

        handler.sendEmptyMessage(READ_FILE_OK);
    }

    private class MidiTrackEventQueue {
        private MidiTrack mTrack;
        private Iterator<MidiEvent> mIterator;
        private ArrayList<MidiEvent> mEventsToDispatch;
        private MidiEvent mNext;

        public MidiTrackEventQueue(MidiTrack track) {
            mTrack = track;

            mIterator = mTrack.getEvents().iterator();
            mEventsToDispatch = new ArrayList<MidiEvent>();

            if(mIterator.hasNext()) {
                mNext = mIterator.next();
            }
        }

        public ArrayList<MidiEvent> getNextEventsUpToTick(double tick) {
            mEventsToDispatch.clear();

            while(mNext != null) {
                if(mNext.getTick() <= tick) {
                    mEventsToDispatch.add(mNext);

                    if(mIterator.hasNext()) {
                        mNext = mIterator.next();
                    } else {
                        mNext = null;
                    }
                } else {
                    break;
                }
            }

            return mEventsToDispatch;
        }

        public boolean hasMoreEvents()
        {
            return mNext != null;
        }
    }
}
