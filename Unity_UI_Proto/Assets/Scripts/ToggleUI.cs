using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleUI : MonoBehaviour
{
    public enum PanelSide
    {
        NONE,
        RIGHT,
        LEFT,
        TOP,
        DOWN
    }

    private float openX;
    private float closedX;
    private bool state = true; //False = closed, True = open

    public PanelSide side = PanelSide.NONE;

    private RectTransform trans;

    // Start is called before the first frame update
    void Start()
    {
        trans = transform.GetComponent<RectTransform>();
        //Debug.Log(this.GetComponent<RectTransform>().rect.x = );
        openX = trans.localPosition.x;
        closedX = trans.localPosition.x - (side == PanelSide.LEFT ? trans.rect.width : -trans.rect.width);
        Debug.Log(openX + " / " + closedX);
    }

    // Update is called once per frame
    void Update()
    {
        //Debug.Log(transform.position);
        //this.transform.position += Vector3.right * Time.deltaTime;
    }

    public void ToggleMovement()
    {
        state = !state;
        trans.localPosition = new Vector3(state ? openX : closedX, trans.localPosition.y, trans.localPosition.z);
        //Vector3(-535.710266,-0.0996910036,0)
        //Vector3(-388.050232,-0.0996910036,0)
    }
}
