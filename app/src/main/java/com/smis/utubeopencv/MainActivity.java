package com.smis.utubeopencv;

import android.graphics.Color;
import android.graphics.Interpolator;
import android.graphics.Point;
import android.graphics.drawable.ColorDrawable;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.Matrix;
import android.os.Environment;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RelativeLayout;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.rajawali3d.surface.IRajawaliSurface;
import org.rajawali3d.surface.RajawaliSurfaceView;

public class MainActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2, View.OnClickListener {

    private static String TAG = "MainActivity";
    private JavaCameraView javaCameraView;
    private Mat mRgba, mGray, mSilhoutte;
    private MarshMallowPermission marshMallowPermission = new MarshMallowPermission(this);
    private Button btnLearn, btnDebug, btnFilterAlgo;
    private boolean debugMode = false;
    private String algo = OpencvNativeClass.ALGO_GAUSSIAN;
    private Renderer renderer;

    private Point actualWindowSize;
    private int javaCameraViewWidth = 640;
    private int javaCameraViewHeight = 480;

    static {
        System.loadLibrary("MyOpencvLibs");
    }

    BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case BaseLoaderCallback.SUCCESS:
                    if (!marshMallowPermission.checkPermissionForCamera()) {
                        marshMallowPermission.requestPermissionForCamera();
                    } else if (!marshMallowPermission.checkPermissionForReadExternalStorage()) {
                        marshMallowPermission.requestPermissionForReadExternalStorage();
                    } else if (!marshMallowPermission.checkPermissionForWriteExternalStorage()) {
                        marshMallowPermission.requestPermissionForWriteExternalStorage();
                    } else {
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

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        Display display = getWindowManager().getDefaultDisplay();
        actualWindowSize = new Point();
        display.getSize(actualWindowSize);

        Log.d("ActualWin", Integer.toString(actualWindowSize.x) + " " + Integer.toString(actualWindowSize.y));

        // NOTE: calculation of actualWindowSize before javaCameraView.setMaxFrameSize is important

        javaCameraView = (JavaCameraView) findViewById(R.id.java_camera_view);
        javaCameraView.setVisibility(View.VISIBLE);
        javaCameraView.setCvCameraViewListener(this);
        javaCameraView.setMaxFrameSize(javaCameraViewWidth, javaCameraViewHeight);


        final RajawaliSurfaceView surface = new RajawaliSurfaceView(this);
        surface.setFrameRate(60.0);
        surface.setRenderMode(IRajawaliSurface.RENDERMODE_WHEN_DIRTY);
        surface.setTransparent(true);
//        surface.setBackgroundColor(Color.MAGENTA);

        // Add mSurface to your root view
        int surfaceHeight = actualWindowSize.y;
        int surfaceWidth = (int) (((float) javaCameraViewWidth / (float) javaCameraViewHeight) * surfaceHeight);
        addContentView(surface, new RelativeLayout.LayoutParams(surfaceWidth, surfaceHeight));
        surface.setX(actualWindowSize.x / 2 - surfaceWidth / 2);


        renderer = new Renderer(this);
        surface.setSurfaceRenderer(renderer);


        btnLearn = (Button) findViewById(R.id.btnLearn);
        btnLearn.setOnClickListener(this);
        btnDebug = (Button) findViewById(R.id.btnDebug);
        btnDebug.setOnClickListener(this);
        btnFilterAlgo = (Button) findViewById(R.id.btnFilterAlgo);
        btnFilterAlgo.setOnClickListener(this);

        String absPath = Environment.getExternalStorageDirectory().getAbsolutePath();
        OpencvNativeClass.initialise(absPath);

        boolean learningMode = OpencvNativeClass.getLearningMode();
        if (learningMode) {
            btnLearn.setText("Learning: ON ");
        } else {
            btnLearn.setText("Learning: OFF");
        }

        if (debugMode) {
            btnDebug.setText("Debug: ON ");
        } else {
            btnDebug.setText("Debug: OFF");
        }

        if (algo.equalsIgnoreCase(OpencvNativeClass.ALGO_GAUSSIAN)) {
            btnFilterAlgo.setText("Gaussian");
        } else if (algo.equalsIgnoreCase(OpencvNativeClass.ALGO_HISOGRAM)) {
            btnFilterAlgo.setText("Histogram");
        } else {
            btnFilterAlgo.setText("Mixed");
        }

        Log.d("NATIVE LOG", "Initialisation complete");
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (javaCameraView != null)
            javaCameraView.disableView();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (javaCameraView != null)
            javaCameraView.disableView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (OpenCVLoader.initDebug()) {
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
        mSilhoutte = new Mat(height, width, CvType.CV_8UC1);

        Log.d("NATIVE-LOG width ", Integer.toString(width));
        Log.d("NATIVE-LOG height ", Integer.toString(height));

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

        if (debugMode) {
            OpencvNativeClass.getHandRegion(mRgba.getNativeObjAddr(), mSilhoutte.getNativeObjAddr());
            return mSilhoutte;
        } else {
            OpencvNativeClass.getHandRegion(mRgba.getNativeObjAddr(), mSilhoutte.getNativeObjAddr());
            float ringX =  ((float)actualWindowSize.y)*(OpencvNativeClass.getRingPositionX()/javaCameraViewHeight);
            float ringY =  ((float)actualWindowSize.y)*(OpencvNativeClass.getRingPositionY()/javaCameraViewHeight);
            renderer.setRingPosition(ringX, ringY);
            return mRgba;
        }

    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnLearn:
                boolean learningMode = OpencvNativeClass.getLearningMode();
                learningMode = !learningMode;
                OpencvNativeClass.setLearningMode(learningMode);
                if (learningMode) {
                    btnLearn.setText("Learning: ON ");
                } else {
                    btnLearn.setText("Learning: OFF");
                }
                break;
            case R.id.btnDebug:
                debugMode = !debugMode;
                if (debugMode) {
                    btnDebug.setText("Debug: ON ");
                } else {
                    btnDebug.setText("Debug: OFF");
                }
                break;
            case R.id.btnFilterAlgo:
                if (algo.equalsIgnoreCase(OpencvNativeClass.ALGO_GAUSSIAN)) {
                    algo = OpencvNativeClass.ALGO_HISOGRAM;
                    btnFilterAlgo.setText("Histogram");
                } else if (algo.equalsIgnoreCase(OpencvNativeClass.ALGO_HISOGRAM)) {
                    algo = OpencvNativeClass.ALGO_MIXED;
                    btnFilterAlgo.setText("Mixed");
                } else {
                    algo = OpencvNativeClass.ALGO_GAUSSIAN;
                    btnFilterAlgo.setText("Gaussian");
                }
                OpencvNativeClass.setFilterAlgo(algo);
                break;
        }

    }
}
