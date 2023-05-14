package io.grpc.helloworldexample;

import java.util.List;

public class ReturnMess {
    List<List<MyPoint>> resultQuery;
    double[] distance;
    List<String> ID;
    ReturnMess(List<List<MyPoint>> resultQuery,double[] distance,List<String> ID) {
        this.distance = distance;
        this.ID = ID;
        this.resultQuery = resultQuery;
    }
}
