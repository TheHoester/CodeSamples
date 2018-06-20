using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class OpenCVInput : MonoBehaviour
{
    public static OpenCVInput Instance
    {
        get
        {
            if (instance == null)
                instance = new OpenCVInput();
            return instance;
        }
    }
    private static OpenCVInput instance;

    [SerializeField]
    private float deadzoneSize = 50.0f;


    private Vector2 cameraResolution;
    private Vector2 viewportSize;

    // Face Detection Variables

    public static List<Vector2> NormalizedFacePositions { get; private set; }
    public static List<int> FaceRadius { get; private set; }
    private bool ready;
    private int maxFaceDetectCount = 5;
    private CvCircle[] faces;

    // Hand Detection Variables
    public static Vector2 PalmPos { get; private set; }
    public static int Fingers { get; private set; }

    // Setting Variables
    public bool IsFace { get; set; }
    public bool IsHand { get; set; }

    void Start()
    {
        instance = this;

        viewportSize.y = Camera.main.orthographicSize * 2.0f;
        viewportSize.x = viewportSize.y * Screen.width / Screen.height;

        int camWidth = 0, camHeight = 0;
        int result = OpenCVInterop.Init(ref camWidth, ref camHeight);
        if (result < 0)
        {
            if (result == -1)
                Debug.LogWarningFormat("[{0}] Failed to find cascades definition.", GetType());
            else if (result == -2)
                Debug.LogWarningFormat("[{0}] Failed to open camera stream.", GetType());

            return;
        }

        cameraResolution = new Vector2(camWidth, camHeight);
        faces = new CvCircle[maxFaceDetectCount];
        NormalizedFacePositions = new List<Vector2>();
        FaceRadius = new List<int>();
        OpenCVInterop.SetScale(1);

        PalmPos = new Vector2();
        Fingers = 0;

        IsFace = false;
        IsHand = false;

        ready = true;
    }

    void OnApplicationQuit() { if (ready) OpenCVInterop.Close(); }

    void Update()
    {
        if (!ready) return;

        if (IsFace)
        {
            int detectedFaceCount = 0;
            unsafe
            {
                fixed (CvCircle* outFaces = faces)
                {
                    OpenCVInterop.DetectFace(outFaces, maxFaceDetectCount, ref detectedFaceCount);
                }
            }

            NormalizedFacePositions.Clear();
            FaceRadius.Clear();
            for (int i = 0; i < detectedFaceCount; i++)
            {
                float x = faces[i].X;
                float y = faces[i].Y;

                if (x < deadzoneSize / 2)
                    x = deadzoneSize / 2;
                else if (x > cameraResolution.x - (deadzoneSize / 2))
                    x = cameraResolution.x - (deadzoneSize / 2);

                if (y < deadzoneSize / 2)
                    y = deadzoneSize / 2;
                else if (y > cameraResolution.y - (deadzoneSize / 2))
                    y = cameraResolution.y - (deadzoneSize / 2);

                NormalizedFacePositions.Add(new Vector2((((x * viewportSize.x) / (cameraResolution.x - deadzoneSize)) - (viewportSize.x / 2)) * -1,
                                                        (((y * viewportSize.y) / (cameraResolution.y - deadzoneSize)) - (viewportSize.y / 2)) * -1));
                FaceRadius.Add(faces[i].Radius);
            }
        }

        if (IsHand)
        {
            double palmX = 0.0d;
            double palmY = 0.0d;
            int numFinger = 0;

            OpenCVInterop.DetectHand(ref palmX, ref palmY, ref numFinger);
            PalmPos = new Vector2(((((float)palmX * viewportSize.x) / cameraResolution.x) - (viewportSize.x / 2)) * -1, 
                                  ((((float)palmY * viewportSize.y) / cameraResolution.y) - (viewportSize.y / 2)) * -1);
            Fingers = numFinger;
        }
    }
}

// Define the functions which can be called from the .dll.
internal static class OpenCVInterop
{
    [DllImport("AdvanGameDev")]
    internal static extern int Init(ref int outCameraWidth, ref int outCameraHeight);

    [DllImport("AdvanGameDev")]
    internal static extern int Close();

    [DllImport("AdvanGameDev")]
    internal static extern int SetScale(int downscale);

    [DllImport("AdvanGameDev")]
    internal unsafe static extern void DetectFace(CvCircle* outFaces, int maxOutFacesCount, ref int outDetectedFacesCount);

    [DllImport("AdvanGameDev")]
    internal static extern void DetectHand(ref double outPalmCentreX, ref double outPalmCentreY, ref int outNumOfFingers);

    [DllImport("AdvanGameDev")]
    internal static extern void DetectHandV2(ref double outPalmCentreX, ref double outPalmCentreY, ref int outNumOfFingers);
}

// Define the structure to be sequential and with the correct byte size (3 ints = 4 bytes * 3 = 12 bytes)
[StructLayout(LayoutKind.Sequential, Size = 12)]
public struct CvCircle
{
    public int X, Y, Radius;
}