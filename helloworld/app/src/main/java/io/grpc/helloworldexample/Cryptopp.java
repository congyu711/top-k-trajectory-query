package io.grpc.helloworldexample;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Cryptopp {
    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("cryptopp");
        System.loadLibrary("grpc_app");
    }

    private static native String[] stringFromJNI();
    private static native String DecryptPhi(byte[] DecryptionPhi,String key);
    private static native EncodePoint[] hilbert(Phi phi, String CS1PublicKey, MyPoint[] query);
    private static native String DecryptID(String DOPublicKey,String[] QUKey,Cap cap,String conversionKey,byte[] kid);
    private static native MyPoint[] DecryptRes(EncodePoint[] Enc_res, Phi phi, String QUPrivateKey);

    private static final Object lock = new Object();

    public static String[] getPublicKey() {
        synchronized (lock) {
            return stringFromJNI();
        }
    }

    static Phi DecryptDO(List<byte[]> Decryption,String key) {
        synchronized (lock) {
            String[] para = new String[5];
            for (int i=0;i<5;i++) {
                para[i] = DecryptPhi(Decryption.get(i),key);
            }
            int order = Integer.parseInt(para[0]);
            int x = Integer.parseInt(para[1]);
            int y = Integer.parseInt(para[2]);
            char dir = para[3].charAt(0);
            double gamma = Double.parseDouble(para[4]);
            return new Phi(order,x,y,dir,gamma);
        }
    }

    static EncodePoint[] hilbertEncode(Phi phi, String CS1PublicKey, MyPoint[] query) {
        synchronized (lock) {
            return hilbert(phi,CS1PublicKey,query);
        }
    }

    static List<String> DecryptId(String DOPublicKey,String[] QUKey,Cap cap,String conversionKey,List<byte[]> kid) {
        synchronized (lock) {
            List<String> result = new ArrayList<>();
            for(byte[] x:kid) {
                String tmp = DecryptID(DOPublicKey,QUKey,cap,conversionKey,x);
                result.add(tmp);
            }
            return result;
        }
    }

    static List<List<MyPoint>> DecryptResult(List<List<EncodePoint>> Enc_res, Phi phi, String QUPrivateKey) {
        synchronized (lock) {
//            return DecryptRes(Enc_res,phi,QUPrivateKey);
            List<List<MyPoint>> resultArr = new ArrayList<>();
            for (List<EncodePoint> x:Enc_res) {
//                EncodePoint[] tmp = (EncodePoint[]) x.toArray();
                EncodePoint[] tmp = new EncodePoint[x.size()];
                for (int i=0;i<x.size();i++) {
                    tmp[i] = x.get(i);
                }
                MyPoint[] result = DecryptRes(tmp,phi,QUPrivateKey);
                List<MyPoint> res = new ArrayList<>(Arrays.asList(result));
                resultArr.add(res);
            }
            return resultArr;
        }
    }
}
