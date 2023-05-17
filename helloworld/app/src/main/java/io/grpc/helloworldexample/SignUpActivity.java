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

import java.util.Objects;
import java.util.concurrent.ExecutionException;

public class SignUpActivity extends AppCompatActivity {
    private EditText userName;
    private EditText password;
    private EditText ensurePassword;

    private InfoViewModel infoViewModel;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.signup);
        Button signUpButton = (Button) findViewById(R.id.register);
        userName = (EditText) findViewById(R.id.username1);
        password = (EditText) findViewById(R.id.password1);
        ensurePassword = (EditText) findViewById(R.id.password2);
        infoViewModel = ViewModelProviders.of(this).get(InfoViewModel.class);
    }

    public void register(View view) throws ExecutionException, InterruptedException {
        if(!userName.getText().toString().equals("")&&!password.getText().toString().equals("")&&!ensurePassword.getText().toString().equals("")) {
            if (password.getText().toString().equals(ensurePassword.getText().toString())) {
                String res = "";
                res = infoViewModel.search(userName.getText().toString());
                while (res == "") {}
                System.out.println("res1 = "+res);
                if (res != null) {
                    Toast.makeText(this, "the username has exited", Toast.LENGTH_SHORT).show();
                } else {
                    infoViewModel.insert(new Info(userName.getText().toString(), password.getText().toString()));
                    Intent intent = new Intent(this, LoginActivity.class);
                    startActivity(intent);
                }
            } else {
                Toast.makeText(this, "Inconsistent password Twice", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "there is empty element", Toast.LENGTH_SHORT).show();
        }
    }
}
