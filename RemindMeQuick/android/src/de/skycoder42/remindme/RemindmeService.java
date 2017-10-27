package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.Binder;
import org.qtproject.qt5.android.bindings.QtService;

public class RemindmeService extends QtService {
	public class LocalBinder extends Binder {
		//empty class
	}

	private final IBinder _binder = new LocalBinder();

	public static void startService(Context context) {
		context.startService(new Intent(context, RemindmeService.class));
	}

	@Override
	public IBinder onBind(Intent intent) {
		return _binder;
	}
}
