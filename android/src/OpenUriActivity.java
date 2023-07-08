package org.kde.tokodon;

import android.content.ContentResolver;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.net.Uri;
import android.app.Activity;

import java.io.FileNotFoundException;

import org.qtproject.qt5.android.bindings.QtActivity;

class AndroidUtils
{
    public static native void openUri(String uri);
}

public class OpenUriActivity extends QtActivity
{
    private static boolean isIntentPending;
    private static boolean isInitialized;
    private static String storedIntentUri;

    private void processIntent() {
        Intent intent = getIntent();
        if ("android.intent.action.VIEW".equals(intent.getAction()) ||
            "android.intent.action.EDIT".equals(intent.getAction())) {
            Uri intentUri = intent.getData();
            if (intentUri != null) {
                storedIntentUri = intentUri.toString();
            }
        }
    }

    public static void checkPendingIntents() {
        isInitialized = true;
        if (isIntentPending) {
            isIntentPending = false;
            sendIntent();
        }
    }

    public static void sendIntent() {
        AndroidUtils.openUri(storedIntentUri.toString());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();
        if (intent != null) {
            String action = intent.getAction();
            if (action != null) {
                isIntentPending = true;
                processIntent();
            }
        }
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
        if (isInitialized) {
            sendIntent();
        } else {
            processIntent();
            isIntentPending = true;
        }
    }
}
