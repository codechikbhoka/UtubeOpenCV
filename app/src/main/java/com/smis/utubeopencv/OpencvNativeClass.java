package com.smis.utubeopencv;

/**
 * Created by gomchik-1404 on 7/10/16.
 */

public class OpencvNativeClass {

    public native static int convertGray(long matAddrRgba, long matAddrGray);

    public native static boolean initialise(String absPath);

    public native static int getHandRegion(long matAddrSrc, long matAddrTarget);

}
