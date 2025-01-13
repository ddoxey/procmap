$(document).ready(function () {
    const socket = io();

    // Initialize Cytoscape
    const cy = cytoscape({
        container: document.getElementById('cy'),
        style: [
            {
                selector: 'node',
                style: {
                    'shape': 'round-rectangle', // Use a rectangle with rounded corners
                    'width': '100px', // Customize the node size
                    'height': '50px',
                    'background-color': '#0074D9', // Node background color
                    'label': 'data(label)', // Use node data for labels
                    'color': '#ffffff', // Text color
                    'text-valign': 'center', // Align text vertically
                    'text-halign': 'center', // Align text horizontally
                    'font-size': '12px', // Font size for labels
                    'border-width': 2, // Optional: Add a border for the nodes
                    'border-color': '#0057A0', // Border color
                }
            },
            {
                selector: 'edge',
                style: {
                    'width': 2,
                    'line-color': '#FF851B',
                    'target-arrow-color': '#FF851B',
                    'target-arrow-shape': 'triangle',
                    'curve-style': 'bezier',
                    'label': 'data(label)', // Display edge label
                    'text-rotation': 'autorotate', // Align text with edge direction
                    'font-size': '10px',
                    'text-background-opacity': 1,
                    'text-background-color': '#ffffff',
                    'text-background-padding': 3,                    
                }
            }
        ],
        layout: {
            name: 'circle'
        }
    });

    // Fetch graph data
    socket.emit('get_data');
    socket.on('graph_data', function (data) {
        cy.add(data.nodes);
        cy.add(data.edges);
        cy.layout({ name: 'circle' }).run();
        // Resize and fit Cytoscape to the container
        cy.resize();
        cy.fit();    
    });
});
