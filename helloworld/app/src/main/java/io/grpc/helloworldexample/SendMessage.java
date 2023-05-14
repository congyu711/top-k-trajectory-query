package io.grpc.helloworldexample;

import java.util.List;

public class SendMessage {
    List<MyPoint> searchQuery;
    String[] QU_Key;
    String k;
    public SendMessage(List<MyPoint> _searchQU,String[] _Key,String K) {
        this.searchQuery = _searchQU;
        this.QU_Key = _Key;
        this.k = K;
    }
}
