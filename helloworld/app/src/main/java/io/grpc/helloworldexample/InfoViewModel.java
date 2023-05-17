package io.grpc.helloworldexample;

import android.app.Application;
import android.arch.lifecycle.AndroidViewModel;
import android.arch.lifecycle.LiveData;

import java.util.List;
import java.util.concurrent.ExecutionException;

public class InfoViewModel extends AndroidViewModel {
    private InfoRepository mRepository;

    private LiveData<List<Info>> mAllWords;

    public InfoViewModel (Application application) {
        super(application);
        mRepository = new InfoRepository(application);
        mAllWords = mRepository.getAllWords();
    }

    LiveData<List<Info>> getAllWords() { return mAllWords; }

    public void insert(Info word) { mRepository.insert(word); }

    public String search(String name) throws ExecutionException, InterruptedException { return mRepository.search(name); }
}
