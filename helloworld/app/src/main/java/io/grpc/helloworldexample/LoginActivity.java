package io.grpc.helloworldexample;

import android.arch.lifecycle.ViewModelProviders;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.util.concurrent.ExecutionException;

public class LoginActivity extends AppCompatActivity {
    private EditText userName;

    private EditText password;

    public InfoViewModel infoViewModel;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);
        Button loginButton = findViewById(R.id.login);
        Button register = findViewById(R.id.signup);
        userName = (EditText) findViewById(R.id.username);
        password = (EditText) findViewById(R.id.password);
        infoViewModel = ViewModelProviders.of(this).get(InfoViewModel.class);
    }

    public void signup(View view) {
        Intent intent = new Intent(this,SignUpActivity.class);
        startActivity(intent);
    }

    public void login(View view) throws ExecutionException, InterruptedException {
        if(!userName.getText().toString().equals("")&&!password.getText().toString().equals("")) {
            String res = "";
            res = infoViewModel.search(userName.getText().toString());
            while (res == "") {}
            System.out.println("res = "+res);
            if(res == null) {
                Toast.makeText(this, "the username does not exist,please register", Toast.LENGTH_SHORT).show();
            } else if (password.getText().toString().equals(res)) {
                Intent intent = new Intent(this,HelloworldActivity.class);
                startActivity(intent);
            } else {
                Toast.makeText(this, "the password is wrong", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "there is empty element", Toast.LENGTH_SHORT).show();
        }
    }
}
