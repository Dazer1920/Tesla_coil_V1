package com.example.teslacoil.View;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.teslacoil.R;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.example.teslacoil.Utils.Pref.getData;
import static com.example.teslacoil.Utils.Pref.saveData;
import static com.example.teslacoil.Utils.Utils.setListViewHeightBasedOnChildren;
import static com.example.teslacoil.View.Fragments.MidiFragment.OPEN_FILE_ACTIV_CODE;
import static com.example.teslacoil.View.Fragments.MidiFragment.OPEN_FILE_RESULT;

public class FileManager extends AppCompatActivity {
    private FilenameFilter filenameFilter;
    private String CurrentDir;

    private TextView directoryText, backText;
    private ListView fileList;
    private String Result = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_manager);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        directoryText = (TextView) findViewById(R.id.directText);
        backText = (TextView) findViewById(R.id.backText);

        fileList = (ListView) findViewById(R.id.filesList);

        setFilter(".*\\.mid|.*\\.midi");

        CurrentDir = getData("DirKey", Environment.getExternalStorageDirectory().getPath());

        directoryText.setText(CurrentDir);
        setBackText();
        setOnClickTitem();
        showNewDirrectory();
    }

    private void setBackText() {
        backText.setText("../");
        backText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File file = new File(CurrentDir);
                File parentDirectory = file.getParentFile();
                if (parentDirectory != null) {
                    CurrentDir = parentDirectory.getPath();
                    directoryText.setText(CurrentDir);
                    showNewDirrectory();
                }
            }
        });
    }

    private void setFilter(final String filter) {
        filenameFilter = new FilenameFilter() {
            @Override
            public boolean accept(File file, String fileName) {
                Pattern filt = Pattern.compile(filter, Pattern.CASE_INSENSITIVE);
                File tempFile = new File(String.format("%s/%s", file.getPath(), fileName));
                if (tempFile.isFile()) {
                    Matcher m = filt.matcher(tempFile.getName());
                    return m.matches();
                }
                return true;
            }
        };
    }

    private void setOnClickTitem() {
        fileList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String info = ((TextView) view).getText().toString();
                String currentDirectory = CurrentDir;
                File file = new File(  currentDirectory + "/" + info);

                if(file.isDirectory()) {
                    CurrentDir = file.getPath();
                    directoryText.setText(CurrentDir);
                    showNewDirrectory();
                }
                else {
                    saveData("DirKey", file.getParent());
                    Result = file.getAbsolutePath();
                    finish();
                }
            }
        });
    }

    private void showNewDirrectory() {
        try {
            File f = new File(CurrentDir);

            File[] files = f.listFiles(filenameFilter);
            if (files == null) files = new File[]{};

            String[] result = new String[files.length];
            for (int i = 0; i < files.length; i++) {
                result[i] = files[i].getName();
            }

            Arrays.sort(result);
            ArrayAdapter<String> files_adatter = new ArrayAdapter<String>(fileList.getContext(),
                    android.R.layout.simple_list_item_1, result);

            fileList.setAdapter(files_adatter);
            setListViewHeightBasedOnChildren(fileList);

        } catch (NullPointerException e) {
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if(id == android.R.id.home) finish();
        return true;
    }


    @Override
    public void finish() {
        Intent intent = new Intent();
        intent.putExtra(OPEN_FILE_RESULT, Result);
        setResult(OPEN_FILE_ACTIV_CODE, intent);

        super.finish();
    }
}
