package org.kde.tokodon;

import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;

import org.qtproject.qt.android.bindings.QtService;

public class TokodonService extends QtService {
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    private final IBinder mBinder = new Binder();
}
