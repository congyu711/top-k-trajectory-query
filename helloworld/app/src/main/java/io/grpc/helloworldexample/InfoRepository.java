package io.grpc.helloworldexample;

import android.app.Application;
import android.arch.lifecycle.LiveData;
import io.grpc.helloworldexample.Info;
import android.os.AsyncTask;

import java.util.List;
import java.util.concurrent.ExecutionException;

public class InfoRepository {
    private InfoDao mWordDao;
    private LiveData<List<Info>> mAllWords;

   InfoRepository(Application application) {
        InfoDataBase db = InfoDataBase.getDatabase(application);
        mWordDao = db.infoDao();
        mAllWords = mWordDao.getAllInfo();
    }

    LiveData<List<Info>> getAllWords() {
        return mAllWords;
    }

    public void insert (Info word) {
        new insertAsyncTask(mWordDao).execute(word);
    }

    public String search (String name) throws ExecutionException, InterruptedException {
       return new searchAsyncTask(mWordDao).execute(name).get();
    }

    private static class searchAsyncTask extends AsyncTask<String,Void, String> {
       private InfoDao mDao;

       searchAsyncTask(InfoDao dao) {
           mDao = dao;
       }

       @Override
       protected String doInBackground(final String... params) {
            int tmp = 0;
           int len = params[0].length();
           for (int i=0;i<len;i++) {
               tmp += params[0].charAt(i);
           }
            return mDao.search(tmp);
       }
    }

    private static class insertAsyncTask extends AsyncTask<Info, Void, Void> {

        private InfoDao mAsyncTaskDao;

        insertAsyncTask(InfoDao dao) {
            mAsyncTaskDao = dao;
        }

        @Override
        protected Void doInBackground(final Info... params) {
            mAsyncTaskDao.insert(params[0]);
            return null;
        }
    }
}
