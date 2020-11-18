package com.example.teslacoil.View.Fragments;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Fragment;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.core.app.ActivityCompat;

import com.example.teslacoil.Controlers.InterrupterController;
import com.example.teslacoil.Controlers.MidiPlayerController;
import com.example.teslacoil.Controlers.MidiUtils.midi.SendTracks;
import com.example.teslacoil.Controlers.Utils.ProcessSystemData;
import com.example.teslacoil.R;
import com.example.teslacoil.Utils.Pref;

import java.util.Timer;
import java.util.TimerTask;

import static com.example.teslacoil.Controlers.Tesla.LOW_POWER_PROTECT;
import static com.example.teslacoil.Controlers.Tesla.TERMO_PROTECT;
import static com.example.teslacoil.Utils.Pref.getData;
import static com.example.teslacoil.Utils.Pref.saveData;

public class MidiFragment extends Fragment {
    public static final String[] EXTERNAL_PERMS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
    };
    public static final int EXTERNAL_REQUEST = 3;

    public static final int OPEN_FILE_ACTIV_CODE = 2;
    public static final String OPEN_FILE_RESULT = "opFil#$";

    private static MidiFragment midiFragment;
    private ProgressDialog progressDialog;
    private Button OpenFileButton, StopButt, PlayButt;
    private TextView CurrentFileText, TrackText, VolumeText, EnaleTracks;
    private SeekBar TrackProgress, VolumeProgress, PortamentoSlider, AttackSlider, DecaySlider, SustainSlider, RealeseSlider;
    private CheckBox useVelocityCheck;
    private Handler handlerTrack;
    private ToggleButton portSwitch;
    private Timer timer;

    public static MidiFragment getInstance() {
        return midiFragment;
    }

    public void startTimer() {
        stopTimer();
        timer = new Timer();
        timer.schedule(new ProgressTimer(), 0, 100);
    }

    public void stopTimer() {
        if(timer != null) {
            timer.cancel();
            timer.purge();
        }
    }

    @SuppressLint("HandlerLeak")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_midi, null);
        midiFragment = this;

        CurrentFileText = (TextView) view.findViewById(R.id.addFileText);
        TrackText = (TextView) view.findViewById(R.id.timeTrackText);
        VolumeText = (TextView) view.findViewById(R.id.volumeText);
        EnaleTracks = (TextView) view.findViewById(R.id.setTrackText);

        TrackProgress = (SeekBar) view.findViewById(R.id.progressTrackBar);

        VolumeProgress = (SeekBar) view.findViewById(R.id.volumeProgress);
        VolumeProgress.setOnSeekBarChangeListener(progress);
        VolumeProgress.setProgress(getData("VolKey", 50));

        TrackProgress.setOnSeekBarChangeListener(progress);

        PortamentoSlider = (SeekBar) view.findViewById(R.id.portSlider);
        PortamentoSlider.setOnSeekBarChangeListener(progress);
        PortamentoSlider.setProgress(getData("PortKey", 0));

        OpenFileButton = (Button) view.findViewById(R.id.openFileButton);
        PlayButt = (Button) view.findViewById(R.id.PlayButt);
        StopButt = (Button) view.findViewById(R.id.StopButt);
        setOpenFileButton();

        useVelocityCheck = (CheckBox) view.findViewById(R.id.velocityCheck);
        useVelocityCheck.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Pref.saveData("isUserVolumeKey", isChecked);
                MidiPlayerController.getInstance().setUseVelocity(isChecked);
            }
        });
        useVelocityCheck.setChecked(Pref.getData("isUserVolumeKey", false));

        AttackSlider = (SeekBar) view.findViewById(R.id.attack_slider);
        DecaySlider = (SeekBar) view.findViewById(R.id.decay_slider);
        SustainSlider = (SeekBar) view.findViewById(R.id.sustain_slider);
        RealeseSlider = (SeekBar) view.findViewById(R.id.release_slider);

        AttackSlider.setOnSeekBarChangeListener(progress);
        DecaySlider.setOnSeekBarChangeListener(progress);
        SustainSlider.setOnSeekBarChangeListener(progress);
        RealeseSlider.setOnSeekBarChangeListener(progress);

        AttackSlider.setProgress(getData("AttackKey", 0));
        DecaySlider.setProgress(getData("DecayKey", 0));
        SustainSlider.setProgress(getData("SustainKey", 100));
        RealeseSlider.setProgress(getData("RealeseKey", 0));

        handlerTrack = new Handler() {
            public void handleMessage(android.os.Message msg) {
                if(!TrackProgress.isPressed()) {
                    if (TrackProgress.getProgress() + 100 <= TrackProgress.getMax()) {
                        TrackProgress.setProgress(TrackProgress.getProgress() + 100);
                        return;
                    }
                }
            }
        };

        MidiPlayerController.getInstance().setMidiPlayerStatus(midiPlayerStatus);
        MidiPlayerController.getInstance().startPlayer();

        return view;
    }

    public void setHandlerEvent(Message msg) {
        if(!isAdded()) return;

        switch (msg.arg1) {
            default: break;

            case OPEN_FILE_ACTIV_CODE: MidiPlayerController.getInstance().openFile((String) msg.obj); break;
            case TERMO_PROTECT: termoProtect((boolean) msg.obj); break;
            case LOW_POWER_PROTECT: lowPowerProtect((boolean) msg.obj); break;
        }
    }

    private void termoProtect(boolean isTermoProtect) {
        if(isTermoProtect) Toast.makeText(getActivity(), getString(R.string.high_temp_toast), Toast.LENGTH_LONG).show();
        MidiPlayerController.getInstance().setProtect();
    }

    private void lowPowerProtect(boolean isLowPower) {
        if(isLowPower) Toast.makeText(getActivity(), getString(R.string.low_power_toast), Toast.LENGTH_LONG).show();
        MidiPlayerController.getInstance().setProtect();
    }

    private void setOpenFileButton() {
        OpenFileButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ActivityCompat.requestPermissions(getActivity(), EXTERNAL_PERMS, EXTERNAL_REQUEST);
            }
        });
    }

    private MidiPlayerController.MidiPlayerStatus midiPlayerStatus = new MidiPlayerController.MidiPlayerStatus() {
        @Override
        public void onStart() {
            StopButt.setEnabled(true);
            OpenFileButton.setEnabled(false);
            EnaleTracks.setEnabled(false);
            useVelocityCheck.setEnabled(false);
            PlayButt.setText(R.string.pause_butt_midi);

            startTimer();
        }

        @Override
        public void onPause() {
            boolean isProtect = !(ProcessSystemData.getInstance().isTermoProtect || ProcessSystemData.getInstance().isLowPower);

            OpenFileButton.setEnabled(true);
            EnaleTracks.setEnabled(true);
            useVelocityCheck.setEnabled(true);
            stopTimer();
            PlayButt.setText(R.string.play_butt_midi);

            PlayButt.setEnabled(isProtect);
        }

        @Override
        public void onStop() {
            stopTimer();
            TrackProgress.setProgress(0);
            StopButt.setEnabled(false);
            EnaleTracks.setEnabled(true);
            useVelocityCheck.setEnabled(true);

            OpenFileButton.setEnabled(true);
            PlayButt.setText(R.string.play_butt_midi);

            boolean isProtect = !(ProcessSystemData.getInstance().isTermoProtect || ProcessSystemData.getInstance().isLowPower);
            PlayButt.setEnabled(isProtect);
        }

        @Override
        public void onReadFile() {
            progressDialog = ProgressDialog.show(getActivity(), "", getString(R.string.read_progress));
        }

        @Override
        public void onReadedFile(boolean fail) {
            if(progressDialog != null && progressDialog.isShowing()) progressDialog.dismiss();

            if(fail) {
                Toast.makeText(getActivity(), getString(R.string.error_read), Toast.LENGTH_LONG).show();
                return;
            }

            CurrentFileText.setText(MidiPlayerController.getInstance().currentFile.getName());
            TrackProgress.setMax((int) MidiPlayerController.getInstance().midiReader.getTimeMs());
            MidiPlayerController.getInstance().setStop();

            setButtons();
            setEnableTrackText();
        }

        @Override
        public void onStartTrack() {
            startTimer();
        }

        @Override
        public void onPauseTrack() {
            stopTimer();
        }

        @Override
        public void lostPackage() {
            Toast.makeText(getActivity(), getString(R.string.lost_package), Toast.LENGTH_LONG).show();
        }

        @Override
        public void onProtectOn() {
            MidiPlayerController.getInstance().setPause();
        }

        @Override
        public void onProtectOff() {
            PlayButt.setEnabled(MidiPlayerController.getInstance().isHaveFileDir);
        }
    };

    private SeekBar.OnSeekBarChangeListener progress = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if(seekBar.getId() == VolumeProgress.getId()) {
                VolumeText.setText(getString(R.string.volume) + " " + progress);
                MidiPlayerController.getInstance().setVolume((byte) progress);
            } else
            if(seekBar.getId() == TrackProgress.getId()) {
                updateTimeText();
            } else if(seekBar.getId() == PortamentoSlider.getId()) {
                MidiPlayerController.getInstance().setPortamentoTime((byte) progress);
            } else
            if(seekBar.getId() == AttackSlider.getId()) {
                MidiPlayerController.getInstance().setAttack((byte) progress);
            } else
            if(seekBar.getId() == DecaySlider.getId()) {
                MidiPlayerController.getInstance().setDecay((byte) progress);
            } else
            if(seekBar.getId() == SustainSlider.getId()) {
                MidiPlayerController.getInstance().setSustain((byte) progress);
            } else
            if(seekBar.getId() == RealeseSlider.getId()) {
                MidiPlayerController.getInstance().setRealese((byte) progress);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            if(seekBar.getId() == VolumeProgress.getId()) {
                saveData("VolKey", seekBar.getProgress());
            } else if(seekBar.getId() == TrackProgress.getId()) {
                MidiPlayerController.getInstance().setTickMidiControllerMs(seekBar.getProgress());
            } else if(seekBar.getId() == PortamentoSlider.getId()) {
                saveData("PortKey", seekBar.getProgress());
            } else
                if(seekBar.getId() == AttackSlider.getId()) {
                saveData("AttackKey", seekBar.getProgress());
            } else
                if(seekBar.getId() == DecaySlider.getId()) {
                saveData("DecayKey", seekBar.getProgress());
            } else
                if(seekBar.getId() == SustainSlider.getId()) {
                saveData("SustainKey", seekBar.getProgress());
            } else
                if(seekBar.getId() == RealeseSlider.getId()) {
                saveData("RealeseKey", seekBar.getProgress());
            }
        }
    };

    @SuppressLint("DefaultLocale")
    private void updateTimeText() {
        int totalSeconds = TrackProgress.getProgress() / 1000;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        int totalSecondsMax = (int) (((MidiPlayerController.getInstance().midiReader != null)?
                MidiPlayerController.getInstance().midiReader.getTimeMs() : 0) / 1000);

        int minutesMax = (totalSecondsMax % 3600) / 60;
        int secondsMax = totalSecondsMax % 60;

        TrackText.setText(String.format("%02d:%02d/%02d:%02d", minutes, seconds, minutesMax, secondsMax));
    }

    class ProgressTimer extends TimerTask {
        ProgressTimer() { }
        public void run() {
            handlerTrack.sendEmptyMessage(0);
        }
    }

    private void setEnableTrackText() {
        EnaleTracks.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MidiPlayerController.getInstance().midiReader != null) {
                    SendTracks sendTracks = new SendTracks(getActivity(),
                            MidiPlayerController.getInstance().midiReader.getCurrentFile());
                    sendTracks.setValues(MidiPlayerController.getInstance().midiReader.getEnableTracks());
                    sendTracks.setResultListener(new SendTracks.Result() {
                        @Override
                        public void result(boolean[] r) {
                            progressDialog = ProgressDialog.show(getActivity(), "", getString(R.string.read_progress));
                            MidiPlayerController.getInstance().midiReader.setEnableTracks(r);
                        }
                    });
                    sendTracks.show();
                }
            }
        });
    }

    private void setButtons() {
        StopButt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MidiPlayerController.getInstance().setStop();
            }
        });

        PlayButt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MidiPlayerController.getInstance().isPlay()) MidiPlayerController.getInstance().setPause();
                else MidiPlayerController.getInstance().setPlay();
            }
        });
    }
}