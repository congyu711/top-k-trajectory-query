package io.grpc.helloworldexample;

import android.util.Pair;

import com.google.common.primitives.UnsignedInteger;

public class Phi {
    int order;
    int origin_x;
    int origin_y;
    char direction;
    double gamma;
    Phi(int _order,int _x,int _y,char _dirt,double _gama) {
        this.order = _order;
        this.origin_x = _x;
        this.origin_y = _y;
        this.direction = _dirt;
        this.gamma = _gama;
    }
}
