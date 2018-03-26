package com.unicamp.alantavares.monocularopencv300;

/**
 * Created by alantavares on 27/04/17.
 */

public class OpencvNativeClass {
    public native static void FindFeatures(long matAddrRgba1, long matAddrRgba2);
}
