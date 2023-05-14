package io.grpc.helloworldexample;

public class EncodePoint {
    double time;
    byte[] hPoint;
    EncodePoint(double _time, byte[] _hpoint) {
        this.time = _time;
        this.hPoint = _hpoint;
    }
}
