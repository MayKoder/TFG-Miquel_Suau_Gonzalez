using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MOV_Camera : MonoBehaviour
{
    public Vector3 target = Vector3.zero;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        this.transform.LookAt(target, this.transform.up);

        if(Input.GetMouseButton(0) == true)
        {
            Vector3 direction = this.transform.position - target;
            //direction.
        }
    }
}
