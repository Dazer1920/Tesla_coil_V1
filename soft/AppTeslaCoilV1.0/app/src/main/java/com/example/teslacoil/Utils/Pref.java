package com.example.teslacoil.Utils;

import android.preference.PreferenceManager;

import com.example.teslacoil.View.MainActivity;

import java.util.Set;

public class Pref {
    public void saveStringSet(String key, Set<String> data) {
        PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance())
                .edit().putStringSet(key, data).apply();
    }

    public Set<String> getStringSet(String key, Set<String> defaultVal) {
        return PreferenceManager.
                getDefaultSharedPreferences(MainActivity.getInstance()).getStringSet(key, defaultVal);
    }

    public static void saveData(String key, int data) {
        PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance())
                .edit().putInt(key, data).apply();
    }

    public static int getData(String key, int fault) {
        return PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance()).getInt(key, fault);
    }

    public static void saveData(String key, String data) {
        PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance())
                .edit().putString(key, data).apply();
    }

    public static String getData(String key, String fault) {
        return PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance()).getString(key, fault);
    }

    public static void saveData(String key, boolean data) {
        PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance())
                .edit().putBoolean(key, data).apply();
    }

    public static boolean getData(String key, boolean fault) {
        return PreferenceManager.getDefaultSharedPreferences(MainActivity.getInstance()).getBoolean(key, fault);
    }
}
