package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtActivity;

public class RemindmeActivity extends QtActivity {
	private IBinder _binder;

	private ServiceConnection _connection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName className, IBinder service) {
			_binder = service;
		}

		@Override
		public void onServiceDisconnected(ComponentName className) {
			_binder = null;
		}
	};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(this, RemindmeService.class);
		bindService(intent, _connection, Context.BIND_AUTO_CREATE);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (_binder != null) {
			unbindService(_connection);
			_binder = null;
		}
	}
}
