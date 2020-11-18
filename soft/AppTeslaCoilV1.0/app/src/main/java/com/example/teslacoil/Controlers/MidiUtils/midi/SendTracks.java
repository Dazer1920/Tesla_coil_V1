package com.example.teslacoil.Controlers.MidiUtils.midi;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Point;
import android.os.Build;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;
import android.widget.ListView;

import androidx.annotation.NonNull;

import com.example.teslacoil.Controlers.MidiUtils.midi.event.MidiEvent;
import com.example.teslacoil.Controlers.MidiUtils.midi.event.meta.TrackName;
import com.example.teslacoil.R;

import java.util.Iterator;

public class SendTracks {
    private boolean[] Values;
    private String[] Items;
    private Context context;
    private MidiFile midiFile;
    private int trackCount;

    private AlertDialog dialog;
    private Result result;

    public interface Result {
        public void result(boolean[] r);
    }

    public void setValues(boolean[] v) {
        for(int i = 0; i < Values.length; i++) {
            Values[i] = v[i];
        }
        CreateView();
    }

    public SendTracks(@NonNull Context context, MidiFile midiFile) {
        this.context = context;
        this.midiFile = midiFile;

        Items = new String[midiFile.getTrackCount()];
        Values = new boolean[midiFile.getTrackCount()];
        trackCount = 1;

        readItems();
        CreateView();
    }

    public void show() {
        dialog.show();
    }

    public SendTracks setResultListener(Result r) {
        this.result = r;
        return this;
    }

    private AlertDialog buildDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        builder.setMultiChoiceItems(Items, Values, new DialogInterface.OnMultiChoiceClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i, boolean b) {
                Values[i] = b;
            }
        });
        return builder.create();
    }

    public void CreateView() {
        dialog = buildDialog();
        dialog.setButton(AlertDialog.BUTTON_POSITIVE,
                context.getText(R.string.set_track_dialog_ok), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                result.result(Values);
                dialog.dismiss();
            }
        });

        dialog.setTitle(context.getText(R.string.set_track_dialog_title));

        dialog.setButton(AlertDialog.BUTTON_NEGATIVE,
                context.getText(R.string.set_track_dialog_set_all), (DialogInterface.OnClickListener) null);

        dialog.setButton(AlertDialog.BUTTON_NEUTRAL,
                context.getText(R.string.set_track_dialog_reset_all), (DialogInterface.OnClickListener) null);

        dialog.setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(DialogInterface d) {
                dialog.getButton(AlertDialog.BUTTON_NEGATIVE).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        for(int i = 0; i < Values.length; i++) {
                            ListView listView = dialog.getListView();
                            Values[i] = true;
                            listView.setItemChecked(i, true);
                        }
                    }
                });

                dialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        for(int i = 0; i < Values.length; i++) {
                            ListView listView = dialog.getListView();
                            Values[i] = false;
                            listView.setItemChecked(i, false);
                        }
                    }
                });
            }
        });
    }

    private void readItems() {
        for(int i = 0; i < midiFile.getTrackCount(); i++) {
            MidiTrack track = midiFile.getTracks().get(i);
            Iterator<MidiEvent> it = track.getEvents().iterator();
            String name = null;
            while (it.hasNext()) {
                MidiEvent event = it.next();

                if (event instanceof TrackName) {
                    name = ((TrackName) event).getTrackName();
                    break;
                }
            }

            if(name == null || name == "") {
                name = context.getResources().getString(R.string.set_track_dialog_track) + " " + trackCount;
                trackCount++;
            }

            Items[i] = name;
        }
    }
}
