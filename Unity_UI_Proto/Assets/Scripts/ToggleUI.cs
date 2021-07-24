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

    public Vector3 movementVector = Vector3.right;

    public PanelSide side = PanelSide.NONE;

    private RectTransform trans;

    // Start is called before the first frame update
    void Start()
    {
        trans = transform.GetComponent<RectTransform>();
        //Debug.Log(this.GetComponent<RectTransform>().rect.x = );
        openX = (side == PanelSide.DOWN || side == PanelSide.TOP ? trans.localPosition.y : trans.localPosition.x);
        closedX = openX - GetSideSize();
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
        trans.localPosition = movementVector * (state ? openX : closedX);
        //Vector3(-535.710266,-0.0996910036,0)
        //Vector3(-388.050232,-0.0996910036,0)
    }

    float GetSideSize()
    {
        float ret = 0.0f;
        switch (side)
        {
            case PanelSide.NONE:
                break;

            case PanelSide.RIGHT:
                ret = -trans.rect.width;
                break;
            case PanelSide.LEFT:
                ret = trans.rect.width;
                break;
            case PanelSide.TOP:
                ret = -trans.rect.height;
                break;
            case PanelSide.DOWN:
                ret = trans.rect.height;
                break;

            default:
                break;
        }
        return ret;
    }
}
