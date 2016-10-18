package com.smis.utubeopencv;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;

import org.rajawali3d.lights.DirectionalLight;
import org.rajawali3d.materials.Material;
import org.rajawali3d.materials.methods.DiffuseMethod;
import org.rajawali3d.materials.textures.ATexture;
import org.rajawali3d.materials.textures.Texture;
import org.rajawali3d.math.Matrix4;
import org.rajawali3d.math.vector.Vector3;
import org.rajawali3d.primitives.Sphere;
import org.rajawali3d.renderer.RajawaliRenderer;
import org.rajawali3d.util.GLU;

/**
 * Created by gomchik-1404 on 17/10/16.
 */

public class Renderer extends RajawaliRenderer {

    private Sphere mEarthSphere;
    private DirectionalLight mDirectionalLight;
    private Context context;
    private int[] mViewport;

    public Renderer(Context context) {
        super(context);
        this.context = context;
        setFrameRate(60);
    }

    @Override
    public void initScene() {
        mDirectionalLight = new DirectionalLight(1f, 0, -1.0f);
        mDirectionalLight.setColor(1.0f, 1.0f, 1.0f);
        mDirectionalLight.setPower(2);
        getCurrentScene().addLight(mDirectionalLight);
        getCurrentCamera().setZ(10);

        Material material = new Material();
        material.enableLighting(true);
        material.setDiffuseMethod(new DiffuseMethod.Lambert());
        material.setColorInfluence(0);
        Texture earthTexture = new Texture("Earth", R.drawable.earthtruecolor_nasa_big);
        try {
            material.addTexture(earthTexture);

        } catch (ATexture.TextureException error) {
            Log.d("DEBUG", "TEXTURE ERROR");
        }

        mEarthSphere = new Sphere(1, 24, 24);
        mEarthSphere.setScale(0.3f);
        mEarthSphere.setMaterial(material);
        getCurrentScene().addChild(mEarthSphere);

    }


    @Override
    public void onOffsetsChanged(float xOffset, float yOffset, float xOffsetStep, float yOffsetStep, int xPixelOffset, int yPixelOffset) {

    }

    @Override
    public void onTouchEvent(MotionEvent event) {

    }

    @Override
    public void onRender(final long elapsedTime, final double deltaTime) {
        super.onRender(elapsedTime, deltaTime);
        mEarthSphere.rotate(Vector3.Axis.Y, 1.0);
    }

    public void setRingPosition(float x, float y) {
//        Log.d("NATIVE-LOG x", Float.toString(x));
//        Log.d("NATIVE-LOG y", Float.toString(y));

        mEarthSphere.setScreenCoordinates(x, getViewportHeight() - y, getViewportWidth(), getViewportHeight(), 10);
    }

}
