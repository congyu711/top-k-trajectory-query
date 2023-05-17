package io.grpc.helloworldexample;

import android.arch.persistence.room.Database;
import android.arch.persistence.room.Room;
import android.arch.persistence.room.RoomDatabase;
import android.content.Context;

@Database(entities = {Info.class}, version = 1, exportSchema = false)
public abstract class InfoDataBase extends RoomDatabase {

    public abstract InfoDao infoDao();
    private static InfoDataBase INSTANCE;

    static InfoDataBase getDatabase(final Context context) {
        if (INSTANCE == null) {
            synchronized (InfoDataBase.class) {
                if (INSTANCE == null) {
                    INSTANCE = Room.databaseBuilder(context.getApplicationContext(),
                                    InfoDataBase.class, "Info_database")
                            // Wipes and rebuilds instead of migrating
                            // if no Migration object.
                            // Migration is not part of this practical.
                            .fallbackToDestructiveMigration()
                            .build();
                }
            }
        }
        return INSTANCE;
    }
}