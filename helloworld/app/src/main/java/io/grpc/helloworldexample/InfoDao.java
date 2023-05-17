package io.grpc.helloworldexample;

import android.arch.lifecycle.LiveData;
import android.arch.persistence.room.Dao;
import android.arch.persistence.room.Insert;
import android.arch.persistence.room.Query;

import java.util.List;

@Dao
interface InfoDao {
    @Insert
    void insert(Info info);

    @Query("DELETE FROM info_table")
    void deleteAll();

    @Query("SELECT passWord FROM Info_table WHERE userID == :code LIMIT 1")
    String search(int code);

    @Query("SELECT *FROM Info_table ORDER BY userID ASC")
    LiveData<List<Info>> getAllInfo();
}
