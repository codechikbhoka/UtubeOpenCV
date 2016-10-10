package com.smis.utubeopencv;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class MainActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2{

    private static String TAG = "MainActivity";
    JavaCameraView javaCameraView;
    Mat mRgba, mGray, mSilhoutte;
    MarshMallowPermission marshMallowPermission = new MarshMallowPermission(this);

    static {
        System.loadLibrary("MyOpencvLibs");
    }

    BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status){
                case BaseLoaderCallback.SUCCESS:
                    if (!marshMallowPermission.checkPermissionForCamera()) {
                        marshMallowPermission.requestPermissionForCamera();
                    } else if(!marshMallowPermission.checkPermissionForReadExternalStorage()) {
                        marshMallowPermission.requestPermissionForReadExternalStorage();
                    } else if(!marshMallowPermission.checkPermissionForWriteExternalStorage()) {
                        marshMallowPermission.requestPermissionForWriteExternalStorage();
                    }
                    else {
                        javaCameraView.enableView();
                    }
                    break;
                default:
                    super.onManagerConnected(status);
                    break;
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        javaCameraView = (JavaCameraView) findViewById(R.id.java_camera_view);
        javaCameraView.setVisibility(View.VISIBLE);
        javaCameraView.setCvCameraViewListener(this);
        javaCameraView.setMaxFrameSize(640,480);

        String absPath = Environment.getExternalStorageDirectory().getAbsolutePath();
        OpencvNativeClass.initialise(absPath);
        Log.d("NATIVE LOG", "Initialisation complete");
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (javaCameraView!=null)
            javaCameraView.disableView();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (javaCameraView!=null)
            javaCameraView.disableView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (OpenCVLoader.initDebug()){
            Log.i(TAG, "opencv loaded successfully");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        } else {
            Log.i(TAG, "opencv not loaded");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_11, this, mLoaderCallback);
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat(height, width, CvType.CV_8UC4);
        mGray = new Mat(height, width, CvType.CV_8UC1);
        mSilhoutte = new Mat(height, width, CvType.CV_8UC4);

        Log.d("NATIVE LOG width ", Integer.toString(width));
        Log.d("NATIVE LOG width ", Integer.toString(height));
    }

    @Override
    public void onCameraViewStopped() {
        mRgba.release();
        mGray.release();
        mSilhoutte.release();

    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();

        //OpencvNativeClass.convertGray(mRgba.getNativeObjAddr(), mGray.getNativeObjAddr());
        OpencvNativeClass.getHandRegion(mRgba.getNativeObjAddr(), mSilhoutte.getNativeObjAddr());
        return mSilhoutte;
    }
}
