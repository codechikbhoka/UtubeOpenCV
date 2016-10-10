package com.smis.utubeopencv;

/**
 * Created by gomchik-1404 on 7/10/16.
 */

public class OpencvNativeClass {

    public final static String ALGO_GAUSSIAN = "algo_gaussian";
    public final static String ALGO_HISOGRAM = "algo_histogram";
    public final static String ALGO_MIXED = "algo_mixed";

    public native static int convertGray(long matAddrRgba, long matAddrGray);

    public native static boolean initialise(String absPath);

    public native static int getHandRegion(long matAddrSrc, long matAddrTarget);

    public native static boolean getLearningMode();

    public native static int setLearningMode(boolean learning);

    public native static String getFilterAlgo();

    public native static int setFilterAlgo(String algo);

}
