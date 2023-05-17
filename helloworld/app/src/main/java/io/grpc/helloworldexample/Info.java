package io.grpc.helloworldexample;

import android.arch.persistence.room.ColumnInfo;
import android.arch.persistence.room.Entity;
import android.arch.persistence.room.PrimaryKey;
import android.support.annotation.NonNull;

@Entity(tableName = "Info_table")
public class Info {
    @PrimaryKey(autoGenerate = true)
    @NonNull
    @ColumnInfo(name = "userID")
    int userID;
    String userName;
    String passWord;

    public Info(@NonNull String userName1,@NonNull String passWord1) {
        this.userID = toInt(userName1);
        this.userName = userName1;
        this.passWord = passWord1;
    }

    public Info() {

    }

    private int toInt(String a) {
        int tmp = 0;
        int len = a.length();
        for (int i=0;i<len;i++) {
            tmp += a.charAt(i);
        }
        return tmp;
    }
}
