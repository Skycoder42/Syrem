package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;

import android.os.IBinder;
import android.os.Binder;

import org.qtproject.qt5.android.bindings.QtService;

public class RemindmeService extends QtService {
	private final IBinder _binder = new Binder();

	@Override
	public IBinder onBind(Intent intent) {
		return _binder;
	}
}
