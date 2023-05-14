/*
 * Copyright 2015 The gRPC Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package io.grpc.helloworldexample;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.protobuf.ByteString;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.examples.helloworld.CS1.PublicKeyRequest;
import io.grpc.examples.helloworld.CS1.QUReply;
import io.grpc.examples.helloworld.CS1.QURequest;
import io.grpc.examples.helloworld.DOQU_GreeterGrpc;
import io.grpc.examples.helloworld.ParameterReply;
import io.grpc.examples.helloworld.CS1.QUCS1_GreeterGrpc;
import io.grpc.examples.helloworld.CS1.PublicKeyReply;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class HelloworldActivity extends AppCompatActivity {
  private Button sendButton;

  private Button addButton;

  private final String[] QU_Key = Cryptopp.getPublicKey();

//  private EditText hostEdit;
//  private EditText portEdit;
//  private EditText messageEdit;
  private TextView resultText;

  private EditText IdEdit;

  private EditText yLabelEdit;

  private EditText xLabelEdit;

  private EditText timeEdit;

  private SendMessage sendMessage = new SendMessage(new ArrayList<>(),QU_Key,new String());

  @SuppressLint("SetTextI18n")
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_helloworld);
    sendButton = (Button) findViewById(R.id.send_button);
    addButton = (Button) findViewById(R.id.add);
    timeEdit = (EditText) findViewById(R.id.time_edit_text);
    xLabelEdit = (EditText) findViewById(R.id.xLabel_edit_text);
    yLabelEdit = (EditText) findViewById(R.id.yLabel_edit_text);
    IdEdit = (EditText) findViewById(R.id.ID_edit_text);
    resultText = (TextView) findViewById(R.id.grpc_response_text);
    resultText.setMovementMethod(new ScrollingMovementMethod());
  }

  public void addTrajectory(View view) {
    if (timeEdit.getText().toString().equals("") ||xLabelEdit.getText().toString().equals("") || yLabelEdit.getText().toString().equals("")) {
      Toast.makeText(this, "elements can not be empty", Toast.LENGTH_SHORT).show();
    } else {
      sendMessage.searchQuery.add(new MyPoint(Double.parseDouble(timeEdit.getText().toString()),Integer.parseInt(xLabelEdit.getText().toString()),Integer.parseInt(yLabelEdit.getText().toString())));
      timeEdit.setText("");
      xLabelEdit.setText("");
      yLabelEdit.setText("");
      StringBuilder tmp = new StringBuilder();
      for (int i =0;i<sendMessage.searchQuery.size();i++) {
        tmp.append("Point").append(String.valueOf(i)).append(" ")
                .append("time:").append(sendMessage.searchQuery.get(i).time).append(" ")
                .append("x:").append(sendMessage.searchQuery.get(i).x).append(" ")
                .append("y:").append(sendMessage.searchQuery.get(i).y).append("\n");
      }
      resultText.setText(tmp);
    }
  }

  public void sendMessage(View view) {
    resultText.setText("");
    if (IdEdit.getText().toString().equals("")) {
      Toast.makeText(this, "the k cannot be empty", Toast.LENGTH_SHORT).show();
    } else {
      sendMessage.k = IdEdit.getText().toString();
      new GrpcTask(this)
              .execute(
                      sendMessage
              );
    }
//    new GrpcTask(this)
//        .execute(
//            hostEdit.getText().toString(),
//            messageEdit.getText().toString(),
//            portEdit.getText().toString());
  }

  private static class GrpcTask extends AsyncTask<SendMessage, Void, ReturnMess> {
    private final WeakReference<Activity> activityReference;
    private ManagedChannel channel;
    private ManagedChannel channel1;

    private GrpcTask(Activity activity) {
      this.activityReference = new WeakReference<Activity>(activity);
    }

    @Override
    protected ReturnMess doInBackground(SendMessage... params) {
//      String host = params[0];
//      String message = params[1];
//      String portStr = params[2];
      int message = Integer.parseInt(params[0].k);
      String[] QU_publicKey = params[0].QU_Key;
      List<MyPoint> searchQuery = params[0].searchQuery;
      try {
        channel = ManagedChannelBuilder.forAddress("172.24.162.219",50051).usePlaintext().build();
        channel1 = ManagedChannelBuilder.forAddress("172.24.162.219", 50052).usePlaintext().build();
        DOQU_GreeterGrpc.DOQU_GreeterBlockingStub stub = DOQU_GreeterGrpc.newBlockingStub(channel);
        io.grpc.examples.helloworld.PublicKeyRequest request = io.grpc.examples.helloworld.PublicKeyRequest.newBuilder().setPublicKey(QU_publicKey[0]).build();
        ParameterReply reply = stub.publicKey(request);
        List<byte[]> cipherPhi = new ArrayList<>();
        for (int i=0;i<reply.getParameterCount();i++) {
          byte[] tmp = reply.getParameter(i).toByteArray();
          cipherPhi.add(tmp);
        }
        Phi phi = Cryptopp.DecryptDO(cipherPhi,QU_publicKey[1]);
        QUCS1_GreeterGrpc.QUCS1_GreeterBlockingStub stub1 = QUCS1_GreeterGrpc.newBlockingStub(channel1);
        io.grpc.examples.helloworld.CS1.PublicKeyRequest request1 = PublicKeyRequest.newBuilder().setName(QU_publicKey[0]).build();
        PublicKeyReply reply1 = stub1.publicKey(request1);
        MyPoint[] searchQu = new MyPoint[searchQuery.size()];
        for (int i=0;i<searchQuery.size();i++) {
          searchQu[i] = searchQuery.get(i);
        }
        EncodePoint[] encodeQuery = Cryptopp.hilbertEncode(phi,reply1.getMessage(),searchQu);
        QURequest.Builder requestBuilder = QURequest.newBuilder().setK(message);
        for (EncodePoint x:encodeQuery) {
          QURequest.qu_Enc quEncBuild = QURequest.qu_Enc.newBuilder().setHpoint(ByteString.copyFrom(x.hPoint)).setT(x.time).build();
          requestBuilder.addQuEncs(quEncBuild);
        }
        QURequest request2 = requestBuilder.build();
        QUReply reply2 = stub1.qUSearch(request2);
        Cap cap = new Cap(reply2.getCap().getE(),reply2.getCap().getV(),reply2.getCap().getS());
        List<byte[]> kid = new ArrayList<>();
        for (int i=0;i<reply2.getKidCount();i++) {
          kid.add(reply2.getKid(i).toByteArray());
        }
        double[] D = new double[reply2.getDCount()];
        for (int i=0;i<reply2.getDCount();i++) {
          D[i] = reply2.getD(i);
        }
        List<String> ID = Cryptopp.DecryptId(reply.getPublicKey(),params[0].QU_Key,cap,reply.getConversionKey(),kid);
        List<List<EncodePoint>> enc_res = new ArrayList<>();
        for (QUReply.Enc_Result x:reply2.getEncResultsList()) {
          List<EncodePoint> tmp = new ArrayList<>();
          for (QURequest.qu_Enc y:x.getQueryResultList()) {
            tmp.add(new EncodePoint(y.getT(),y.getHpoint().toByteArray()));
          }
          enc_res.add(tmp);
        }
        List<List<MyPoint>> result = Cryptopp.DecryptResult(enc_res,phi,params[0].QU_Key[1]);
        return new ReturnMess(result,D,ID);
      } catch (Exception e) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        e.printStackTrace(pw);
        pw.flush();
        return new  ReturnMess(null,null, Collections.singletonList(String.format("Failed... : %n%s", sw)));
      }
    }

    @Override
    protected void onPostExecute(ReturnMess result) {
      try {
        channel.shutdown().awaitTermination(1, TimeUnit.SECONDS);
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }
      Activity activity = activityReference.get();
      if (activity == null) {
        return;
      }
      TextView resultText = (TextView) activity.findViewById(R.id.grpc_response_text);
      Button sendButton = (Button) activity.findViewById(R.id.send_button);
      StringBuilder stringBuilder = new StringBuilder();
      for (int i=0;i<result.ID.size();i++) {
        stringBuilder.append("ID:").append(result.ID.get(i)).append(" ");
        for (MyPoint x:result.resultQuery.get(i)) {
          stringBuilder.append("time:").append(x.time).append(" ")
                  .append("x:").append(x.x).append(" ")
                  .append("y:").append(x.y).append(" ");
        }
        stringBuilder.append("\n");
      }
      resultText.setText(stringBuilder);
      sendButton.setEnabled(true);
    }
  }
}

