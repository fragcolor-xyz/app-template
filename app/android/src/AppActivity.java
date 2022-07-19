package xyz.fragcolor.fragments;
import org.libsdl.app.SDLActivity;

public class AppActivity extends SDLActivity {
    protected String[] getLibraries() {
        return  new String[] {
            "App" // libApp.so
        };
    }
}
