package xyz.fragcolor.fragments

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import org.libsdl.app.SDLActivity;

class MainActivity : SDLActivity() {
    override fun getLibraries() : Array<String> {
        return arrayOf(
            "app"
        )
    }

    override fun getMainFunction() :String {
        return "main"
    }
}
